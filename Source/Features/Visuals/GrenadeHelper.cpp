#include "GrenadeHelper.hpp"

#include "../Feature.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"

#include "../../Utils/Projection.hpp"

#include "../../Memory.hpp"

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/hash.hpp" // IWYU pragma: keep

#include "nlohmann/json_fwd.hpp"

#include "imgui.h"

#include "octree-cpp/OctreeCpp.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace Serialization::Grenades;

GrenadeHelper::GrenadeHelper()
	: Feature("Visuals", "Grenade helper")
	// TODO: Verify that this is actually the boundaries that the s2 engine restricts to.
	, grenades({ glm::vec3{ -32768 }, glm::vec3{ 32768 } })
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

void GrenadeHelper::update()
{
	if (!Memory::local_player) {
		const std::lock_guard lock{ proximate_grenades_mutex };
		proximate_grenades.clear();
		return;
	}

	const glm::vec3 origin = Memory::local_player->old_origin();

	const float render_distance = this->render_distance.get();

	auto range = grenades.Query(Octree::Sphere{ .Midpoint = origin, .Radius = render_distance })
		| std::ranges::views::filter([](const Octree::TDataWrapper& data) {
			  return data.Data.contains(GrenadeWeapon::WEAPON_SMOKEGRENADE);
		  })
		| std::ranges::views::transform([&origin, render_distance](const Octree::TDataWrapper& data) {
			  const std::vector<Grenade>& grenades = data.Data.at(GrenadeWeapon::WEAPON_SMOKEGRENADE);

			  std::size_t hash = 0;

			  for (const Grenade& grenade : grenades) {
				  hash = hash ^ (std::hash<Grenade>{}(grenade) << 1);
			  }

			  const float fade = (1.0F - distance(origin, data.Vector) / render_distance) * 0.7F;

			  return GrenadeBundle{
				  .grenades = grenades,
				  .alpha = fade,
				  .position = data.Vector,
				  .hash = hash,
			  };
		  });

	std::vector<GrenadeBundle> bundles(range.begin(), range.end());

	std::ranges::sort(bundles, [&origin](const GrenadeBundle& a, const GrenadeBundle& b) {
		return glm::distance(origin, a.position) < glm::distance(origin, b.position);
	});

	{
		const std::lock_guard lock{ proximate_grenades_mutex };
		std::swap(proximate_grenades, bundles);
	}
}

void GrenadeHelper::event_handler(GameEvent* event)
{
	if (std::string_view{ event->GetName() } != "game_newmap")
		return;

	const std::string_view new_map = event->get_string("mapname");

	if (current_map != new_map) {
		std::unordered_map<glm::vec3, std::unordered_map<GrenadeWeapon, std::vector<Grenade>>> map;
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

			GrenadeWeapon weapon = grenade.weapon;
			auto vec_iter = std::ranges::find_if(map_iter->second, [weapon](const auto& pair) {
				return pair.first == weapon;
			});

			if (vec_iter == map_iter->second.end())
				// new vector
				vec_iter = map_iter->second.insert(vec_iter, { weapon, {} });

			vec_iter->second.emplace_back(std::move(grenade));
		}

		for (const auto& [pos, v] : map) {
			grenades.Add({ .Vector = pos, .Data = { v } });
		}
		current_map = new_map;
	}
}

void GrenadeHelper::draw() const
{
	if (!enabled.get())
		return;

	const std::lock_guard lock{ proximate_grenades_mutex };

	for (const GrenadeBundle& bundle : proximate_grenades) {
		ImVec2 screen_pos;
		if (!Projection::project(bundle.position, screen_pos))
			continue;

		const std::string grenade_id{ "##Grenade" + std::to_string(bundle.hash) };

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, bundle.alpha);
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

			for (const Grenade& grenade : bundle.grenades)
				ImGui::TextUnformatted(grenade.name.to.c_str());
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}
