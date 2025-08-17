#include "GrenadeHelper.hpp"

#include "../Feature.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "../../SDK/Entities/BaseCSGrenade.hpp"
#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/CSPlayerMovementServices.hpp"
#include "../../SDK/Entities/Services/PlayerWeaponServices.hpp"
#include "../../SDK/Enums/GrenadeType.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"

#include "../../Utils/Logging.hpp"
#include "../../Utils/Projection.hpp"

#include "../../Memory.hpp"

#include "../../GUI/GUI.hpp"

#include "glm/common.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/hash.hpp" // IWYU pragma: keep
#include "glm/trigonometric.hpp"

#include "magic_enum/magic_enum.hpp"

#include "nlohmann/json_fwd.hpp"

#include "octree-cpp/OctreeCpp.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <memory>
#include <mutex>
#include <numbers>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace Serialization::Grenades;

GrenadeHelper::GrenadeHelper()
	: Feature("Visuals", "Grenade helper")
	, grenades({ glm::vec3{ -OCTREE_SIZE }, glm::vec3{ OCTREE_SIZE } })
{
}

template <>
struct std::hash<Grenade> {
	std::size_t operator()(const Grenade& grenade) const noexcept
	{
		const std::size_t h1 = std::hash<glm::vec3>{}(grenade.position);
		const std::size_t h2 = std::hash<glm::vec2>{}(grenade.viewangles);
		return h1 ^ (h2 << 1);
	}
};

void GrenadeHelper::clear_current_grenades()
{
	const std::lock_guard lock{ proximate_grenades_mutex };
	proximate_grenades.clear();
}

void GrenadeHelper::update_viewangles(const glm::vec3& viewangles)
{
	const std::lock_guard lock{ proximate_grenades_mutex };
	player_viewangles = viewangles;
}

void GrenadeHelper::update()
{
	if (!enabled.get() || !Memory::local_player) {
		clear_current_grenades();
		return;
	}

	GameSceneNode* game_scene_node = Memory::local_player->gameSceneNode();
	if (!game_scene_node) {
		clear_current_grenades();
		return;
	}

	BasePlayerWeapon* weapon = Memory::local_player->weapon_services()
		? Memory::local_player->weapon_services()->active_weapon().get()
		: nullptr;
	if (!weapon) {
		clear_current_grenades();
		return;
	}

	auto* grenade = weapon->entity_cast<BaseCSGrenade*>();
	if (!grenade) {
		clear_current_grenades();
		return;
	}

	const int grenade_type = grenade->get_grenade_type();

	const auto optional_weapon = magic_enum::enum_cast<GrenadeType>(grenade_type);

	if (!optional_weapon.has_value()) {
		Logging::error("Unknown grenade type: {}", grenade_type);
		clear_current_grenades();
		return;
	}

	const GrenadeType grenade_weapon = optional_weapon.value();

	const glm::vec3 origin = game_scene_node->transform().m_Position;

	const float render_distance = this->render_distance.get();

	auto range = grenades.Query(Octree::Sphere{ .Midpoint = origin, .Radius = render_distance })
		| std::ranges::views::filter([grenade_weapon](const Octree::TDataWrapper& data) {
			  return data.Data.contains(grenade_weapon);
		  })
		| std::ranges::views::transform([&origin, render_distance, grenade_weapon, this](const Octree::TDataWrapper& data) {
			  std::shared_ptr<GrenadeBundle> grenades = data.Data.at(grenade_weapon);

			  glm::vec2 viewangles = player_viewangles.xy(); // No lock needed because this is the same thread as CreateMove (TODO verify.)

			  std::ranges::sort(grenades->grenades, [&viewangles](const Grenade& a, const Grenade& b) {
				  return distance(viewangles, a.viewangles) < distance(viewangles, b.viewangles);
			  });

			  const float fade = (1.0F - distance(origin, data.Vector) / render_distance) * 0.7F;

			  static constexpr float IN_POSITION_THRESHOLD = 1.0F;

			  return ProximateGrenadeBundle{
				  .grenades = std::move(grenades),
				  .alpha = fade,
				  .position = data.Vector,
				  .in_position = distance(data.Vector, origin) < IN_POSITION_THRESHOLD,
			  };
		  });

	std::vector<ProximateGrenadeBundle> bundles(range.begin(), range.end());

	std::ranges::sort(bundles, [&origin](const ProximateGrenadeBundle& a, const ProximateGrenadeBundle& b) {
		return glm::distance(origin, a.position) < glm::distance(origin, b.position);
	});

	CSPlayerMovementServices* movement_services = Memory::local_player->movement_services();

	{
		const std::lock_guard lock{ proximate_grenades_mutex };
		view_offset = Memory::local_player->view_offset();
		ducking = movement_services->ducking() || movement_services->ducked();
		std::swap(proximate_grenades, bundles);
	}
}

void GrenadeHelper::reset_octree()
{
	// eh, maybe make a patch for resetting an octree one day.
	grenades.~Octree();
	new (&grenades) Octree({ glm::vec3{ -OCTREE_SIZE }, glm::vec3{ OCTREE_SIZE } });
}

void GrenadeHelper::event_handler(GameEvent* event)
{
	if (std::string_view{ event->GetName() } != "game_newmap")
		return;

	const std::string_view new_map = event->get_string("mapname", "");

	if (new_map.empty()) {
		Logging::error("Failed to retrieve map name");
		reset_octree();
		return;
	}

	if (current_map == new_map)
		return;

	current_map = new_map;

	std::unordered_map<glm::vec3, std::unordered_map<GrenadeType, std::shared_ptr<GrenadeBundle>>> map;
	for (Grenade grenade : parse_grenades_for_map(new_map)) {
		auto map_iter = std::ranges::find_if(map, [&grenade](const auto& it) {
			// static constexpr float MERGE_DISTANCE = 1E-2F;
			// TODO This is insane, some grenades don't merge with that little
			static constexpr float MERGE_DISTANCE = 1.0F;
			return distance(it.first, grenade.position) < MERGE_DISTANCE;
		});

		if (map_iter == map.end())
			// new map
			map_iter = map.insert(map_iter, { grenade.position, {} });

		GrenadeType weapon = grenade.weapon;
		auto vec_iter = std::ranges::find_if(map_iter->second, [weapon](const auto& pair) {
			return pair.first == weapon;
		});

		if (vec_iter == map_iter->second.end())
			// new vector
			vec_iter = map_iter->second.insert(vec_iter, { weapon, std::make_shared<GrenadeBundle>() });

		auto& counts = vec_iter->second->counts;
		auto it = std::ranges::find(counts, grenade.name.to, [](const auto& pair) { return pair.first; });
		if (it == counts.end())
			it = counts.insert(it, { grenade.name.to, 0 });
		it->second++;

		vec_iter->second->hash ^= (std::hash<Grenade>{}(grenade) << 1);

		vec_iter->second->grenades.emplace_back(std::move(grenade));
	}

	reset_octree();
	for (auto&& [pos, v] : map)
		grenades.Add({ .Vector = pos, .Data = std::move(v) });
}

void GrenadeHelper::draw_surrounded_grenade(const ProximateGrenadeBundle& bundle, ImVec2 screen_pos)
{
	const std::string grenade_id{ "##Grenade" + std::to_string(bundle.grenades->hash) };

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, bundle.alpha);
	if (bundle.in_position)
		ImGui::PushStyleColor(ImGuiCol_Border, { 0.0F, 1.0F, 0.0F, 0.5F });
	ImGui::SetNextWindowPos(screen_pos, ImGuiCond_None, { 0.5F, 0.5F });

	if (ImGui::Begin(grenade_id.c_str(), nullptr,
			ImGuiWindowFlags_NoInputs
				| ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoNav
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoScrollWithMouse
				| ImGuiWindowFlags_NoFocusOnAppearing
				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_AlwaysAutoResize)) {

		for (const auto& [grenade_target, count] : bundle.grenades->counts)
			if (count > 1)
				ImGui::Text("%zux %s", count, grenade_target.c_str());
			else
				ImGui::TextUnformatted(grenade_target.c_str());
	}

	ImGui::BringWindowToDisplayBack(ImGui::GetCurrentWindow());

	ImGui::End();
	if (bundle.in_position)
		ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

void GrenadeHelper::draw_aim_helpers(const Grenade& grenade, ImVec2 screen_pos) const
{
	const glm::vec2 screen_vec{ screen_pos.x, screen_pos.y };
	const ImVec2 screen_center = ImGui::GetIO().DisplaySize / 2.0;
	const glm::vec2 center_vec{ screen_center.x, screen_center.y };

	const float aim_circle_radius = this->aim_circle_radius.get() * GUI::get_scale();

	const float max_distance = glm::length(glm::vec2{ aim_circle_radius, aim_circle_radius });
	const float distance = glm::distance(screen_vec, center_vec);

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.6F + glm::clamp(1.0F - distance / max_distance, 0.0F, 1.0F) * 0.3F);
	ImGui::SetNextWindowPos(screen_pos, ImGuiCond_None, { 1.1F, 0.5F });

	const std::string grenade_id{ "##Grenade_Hint" + std::to_string(std::hash<Grenade>{}(grenade)) };

	if (ImGui::Begin(grenade_id.c_str(), nullptr,
			ImGuiWindowFlags_NoInputs
				| ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoNav
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoScrollWithMouse
				| ImGuiWindowFlags_NoFocusOnAppearing
				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_AlwaysAutoResize)) {

		const char* action = nullptr;
		switch (grenade.weapon) {
		case GRENADE_TYPE_EXPLOSIVE:
			action = "Nade";
			break;
		case GRENADE_TYPE_FLASH:
			action = "Flash";
			break;
		case GRENADE_TYPE_FIRE:
			action = "Molotov";
			break;
		case GRENADE_TYPE_DECOY:
			action = "Deploy decoy at";
			break;
		case GRENADE_TYPE_SMOKE:
			action = "Smoke";
			break;
		case GRENADE_TYPE_SENSOR:
			action = "Monitor";
			break;
		case GRENADE_TYPE_SNOWBALL:
			action = "Throw snowball at";
			break;
		default:
			std::unreachable();
		}

		ImGui::Text("%s %s from %s",
			action,
			grenade.name.to.c_str(),
			grenade.name.from.empty() ? "here" /*saved*/ : grenade.name.from.c_str());

		ImGui::Separator();

		const auto colored_text_if_unfulfilled = [](const char* text, bool fulfilled) {
			if (fulfilled)
				ImGui::TextUnformatted(text);
			else
				ImGui::TextColored(ImColor{ 1.0F, 0.0F, 0.0F, 1.0F }, "%s", text);
		};
		if (grenade.duck)
			colored_text_if_unfulfilled("Crouching", ducking);
		else if (ducking)
			colored_text_if_unfulfilled("Standing", false);
		if (grenade.throw_info.jump)
			// Jump throw is never satisfied, because if you jump then it is no longer rendered.
			// Just assume that it is satisfied.
			ImGui::Text("Jump-throw");

		if (grenade.throw_info.strength == 1.0F)
			colored_text_if_unfulfilled("Left-click",
				ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDown(ImGuiMouseButton_Right));
		else if (grenade.throw_info.strength == 0.0F)
			colored_text_if_unfulfilled("Right-click",
				!ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseDown(ImGuiMouseButton_Right));
		else if (grenade.throw_info.strength == 0.5F)
			colored_text_if_unfulfilled("Left-click + Right-click",
				ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsMouseDown(ImGuiMouseButton_Right));
		else
			ImGui::Text("Throw strength: %f", grenade.throw_info.strength);

		if (!grenade.description.empty())
			ImGui::TextUnformatted(grenade.description.c_str());
	}

	ImGui::BringWindowToDisplayBack(ImGui::GetCurrentWindow());

	ImGui::End();
	ImGui::PopStyleVar();

	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	const float circle_thickness = 1.0F * GUI::get_scale();

	draw_list->PathArcTo(screen_pos, aim_circle_radius, 0.0F, 2.0F * std::numbers::pi_v<float>);
	draw_list->_Path.Size--;
	draw_list->PathStroke(aim_circle_color.get(), ImDrawFlags_Closed, circle_thickness);

	const glm::vec3 needed_viewangles{ grenade.viewangles, 0.0F };

	glm::vec3 aim_diff = needed_viewangles - player_viewangles;
	aim_diff.y = std::remainder(aim_diff.y, 360.0F);

	const float aim_distance = glm::length(aim_diff);

	const float normalized_distance = glm::clamp(
		(aim_distance - grenade.throw_info.aim_tolerance) / (sqrt(aim_circle_radius) * 2 /*x and y*/), 0.0F, 1.0F);

	if (normalized_distance >= 1.0F)
		return;

	draw_list->PathArcTo(screen_pos, aim_circle_radius, 0.0F, 2.0F * std::numbers::pi_v<float> * (1.0F - sqrt(normalized_distance)));
	draw_list->_Path.Size--;
	draw_list->PathStroke(aim_circle_fill_color.get(), ImDrawFlags_None, circle_thickness / 2.0F);
}

void GrenadeHelper::draw() const
{
	if (!enabled.get())
		return;

	const std::lock_guard lock{ proximate_grenades_mutex };

	for (const ProximateGrenadeBundle& bundle : proximate_grenades) {
		ImVec2 screen_pos;
		if (Projection::project(bundle.position, screen_pos))
			draw_surrounded_grenade(bundle, screen_pos);

		if (bundle.in_position) {
			for (const Grenade& grenade : bundle.grenades->grenades) {
				const float pitch = glm::radians(grenade.viewangles.x);
				const float yaw = glm::radians(grenade.viewangles.y);
				const glm::vec3 dir{
					glm::cos(yaw) * glm::cos(pitch),
					glm::sin(yaw) * glm::cos(pitch),
					-glm::sin(pitch),
				};

				const glm::vec3 intended_eye_pos = grenade.position + view_offset;

				const glm::vec3 aim_position = intended_eye_pos + dir * 128.0F; /* Prevent ending up in near clip plane */

				ImVec2 screen_pos;
				if (Projection::project(aim_position, screen_pos))
					draw_aim_helpers(grenade, screen_pos);
			}
		}
	}
}
