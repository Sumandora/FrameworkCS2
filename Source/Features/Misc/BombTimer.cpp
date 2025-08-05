#include "BombTimer.hpp"

#include "../Feature.hpp"

#include "../../SDK/EngineTrace/GameTrace.hpp"
#include "../../SDK/Entities/CSPlayerController.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/GameEntitySystem.hpp"
#include "../../SDK/Entities/MapInfo.hpp"
#include "../../SDK/Entities/PlantedC4.hpp"
#include "../../SDK/EntityHandle.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"
#include "../../SDK/GameClass/GlobalVars.hpp"

#include "../../Memory.hpp"

#include "../../GUI/GUI.hpp"

#include "../../Utils/Logging.hpp"

#include "glm/geometric.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <cmath>
#include <mutex>
#include <optional>

BombTimer::BombTimer()
	: Feature("Misc", "Bomb timer")
{
}

static float calculate_bomb_damage(float bomb_damage, float distance)
{
	const float bomb_radius = bomb_damage * 3.5F;

	const float sigma = bomb_radius / 3.0F;
	const float gaussian_falloff = exp(-distance * distance / (2.0F * sigma * sigma));
	const float adjusted_damage = bomb_damage * gaussian_falloff;

	const float scaled_damage = Memory::local_player->scale_damage_with_armor(adjusted_damage, 1.0F, HitGroup::GENERIC);

	return std::floor(scaled_damage);
}

static bool is_c4_relevant(const PlantedC4* c4)
{
	if (c4->has_exploded())
		return false;

	if (c4->defused())
		return false;

	static constexpr float RELEVANCY_DELAY = 5.0F;
	if (c4->c4_blow() + RELEVANCY_DELAY < (*Memory::globals)->current_time())
		// NOLINTNEXTLINE(readability-simplify-boolean-expr)
		return false; // Since the blow time is before the damage is applied, one should wait a little before removing the information

	return true;
}

PlantedC4* BombTimer::update_planted_c4()
{
	if (planted_c4.has_entity()) {
		const PlantedC4* old_c4 = planted_c4.get();
		if (old_c4 && !is_c4_relevant(old_c4)) {
			planted_c4 = EntityHandle<PlantedC4>::invalid();
		}
	}

	PlantedC4* c4 = nullptr;
	// NOLINTNEXTLINE(bugprone-assignment-in-if-condition)
	if (!planted_c4.has_entity() || (c4 = planted_c4.get()) == nullptr) {
		for (PlantedC4* potential_c4 : GameEntitySystem::the()->entities_of_type<PlantedC4>()) {
			if (!is_c4_relevant(potential_c4))
				continue;

			c4 = potential_c4;
			break;
		}
	}

	if (!c4) {
		return nullptr;
	}

	planted_c4 = c4->get_handle();
	return c4;
}

MapInfo* BombTimer::update_map_info()
{
	MapInfo* map_info = nullptr;
	// NOLINTNEXTLINE(bugprone-assignment-in-if-condition)
	if (!this->map_info.has_entity() || (map_info = this->map_info.get()) == nullptr) {
		map_info = *GameEntitySystem::the()->entities_of_type<MapInfo>().begin();
	}

	return map_info;
}

void BombTimer::clear_bomb_info()
{
	const std::lock_guard lock{ bomb_info_lock };
	bomb_info.reset();
}

void BombTimer::update()
{
	if (!enabled.get() || !Memory::local_player) {
		clear_bomb_info();
		return;
	}

	PlantedC4* c4 = update_planted_c4();
	if (!c4) {
		clear_bomb_info();
		planted_c4 = EntityHandle<PlantedC4>::invalid();
		return;
	}

	MapInfo* map_info = update_map_info();
	if (!map_info) {
		this->map_info = EntityHandle<MapInfo>::invalid();
	}

	float bomb_radius = 500.0F; // Default value
	if (map_info && map_info->bomb_radius() != 0)
		bomb_radius = map_info->bomb_radius();

	const GameSceneNode* local_node = Memory::local_player->gameSceneNode();
	const GameSceneNode* c4_node = c4->gameSceneNode();
	if (!local_node || !c4_node) {
		clear_bomb_info();
		return;
	}

	const float distance = glm::distance(local_node->transform().m_Position, c4_node->transform().m_Position);

	BombInfo my_bomb_info;

	my_bomb_info.current_time = (*Memory::globals)->current_time();
	my_bomb_info.explode_time = c4->c4_blow();
	my_bomb_info.damage = calculate_bomb_damage(bomb_radius, distance);
	my_bomb_info.site = c4->bomb_site();

	if (c4->being_defused()) {
		BombInfo::DefuseInfo defuse_info;

		if (const CSPlayerPawn* defuser = c4->bomb_defuser().get(); defuser) {
			const CSPlayerController* controller = defuser->original_controller().get();
			if (controller) {
				defuse_info.defuser_name = controller->get_decorated_player_name();
			}
		}

		defuse_info.end_time = c4->defuse_count_down();

		my_bomb_info.defuse_info = defuse_info;
	}

	{
		const std::lock_guard lock{ bomb_info_lock };
		this->bomb_info = std::optional<BombInfo>{ my_bomb_info };
	}
}

void BombTimer::draw()
{
	if (!enabled.get())
		return;

	if (!bomb_info.has_value() && !GUI::is_menu_open())
		return;

	if (ImGui::Begin("Bomb timer", nullptr,
			(GUI::is_menu_open() ? 0 : ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove)
				| ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoNav
				| ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoScrollWithMouse
				| ImGuiWindowFlags_NoFocusOnAppearing
				| ImGuiWindowFlags_AlwaysAutoResize)) {
		std::optional<BombInfo> bomb_info;

		{
			const std::lock_guard lock{ bomb_info_lock };
			bomb_info = this->bomb_info;
		}

		if (!bomb_info.has_value() && GUI::is_menu_open()) {
			bomb_info = BombInfo{
				.current_time = 0.0F,
				.explode_time = 5.0F,
				.damage = 101.0F,
				.site = 2,
				.defuse_info = BombInfo::DefuseInfo{
					.defuser_name = "rip_tycho",
					.end_time = 4.0F,
				},
			};
		}

		if (bomb_info.has_value()) {
			const BombInfo& info = bomb_info.value();

			const char site = static_cast<char>('A' + info.site);
			ImGui::Text("Bomb planted on Site %c", site);

			const float remaining_time = info.explode_time - info.current_time;
			if (remaining_time > 0)
				ImGui::Text("Remaining time: %.1f", remaining_time);
			ImGui::Text("Damage: %.0f", info.damage);

			if (info.defuse_info.has_value()) {
				const BombInfo::DefuseInfo& defuse_info = info.defuse_info.value();

				ImGui::Separator();

				if (!defuse_info.defuser_name.empty())
					ImGui::Text("Defused by %s", defuse_info.defuser_name.c_str());
				const float remaining_time_for_defuse = defuse_info.end_time - info.current_time;
				if (remaining_time_for_defuse > 0)
					ImGui::Text("Remaining time: %.1f", remaining_time_for_defuse);

				const bool will_succeed = defuse_info.end_time <= info.explode_time;
				static constexpr ImColor RED{ 1.0F, 0.0F, 0.0F, 1.0F };
				static constexpr ImColor GREEN{ 0.0F, 1.0F, 0.0F, 1.0F };
				ImGui::TextColored(will_succeed ? GREEN : RED, "Will %s", will_succeed ? "succeed" : "fail");
			}
		}
	}

	if (!GUI::is_menu_open())
		ImGui::BringWindowToDisplayBack(ImGui::GetCurrentWindow());

	ImGui::End();
}
