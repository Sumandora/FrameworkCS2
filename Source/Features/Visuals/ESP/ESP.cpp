#include "ESP.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <limits>
#include <set>
#include <string>
#include <utility>

#include "../../../Memory.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/Entities/GameEntitySystem.hpp"
#include "../../../SDK/Enums/LifeState.hpp"
#include "../../../SDK/Enums/TeamID.hpp"
#include "../../../SDK/GameClass/CollisionProperty.hpp"
#include "../../../SDK/GameClass/GameSceneNode.hpp"
#include "../../../SDK/Schema/SchemaClassInfo.hpp"

#include "../../../Utils/BulletSimulation.hpp"
#include "../../../Utils/Projection.hpp"

#include "../../Feature.hpp"

#include "GenericESP/UnionedRect.hpp"

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "Skeleton.hpp"

ESP::ESP()
	: Feature("Visuals", "ESP")
{
}

struct ESPEntity {
	BaseEntity* entity;
	SchemaClassInfo* class_info;
	float distance;
	ImRect screenspace_rect;

	struct DistanceComparator {
		static bool operator()(const ESPEntity& a, const ESPEntity& b)
		{
			// Since we want to draw entities further away first, we sort with greater than
			return a.distance > b.distance;
		}
	};
};

bool ESP::Player::is_enabled() const
{
	if (this == &esp->local) {
		// TODO If we are in first person then never say yes here.
	}
	return box_enabled.get() || name_enabled.get() || healthbar_enabled.get() || skeleton_enabled.get();
}

void ESP::Player::draw_player(ImDrawList* draw_list, CSPlayerPawn* player_pawn, const ImRect& screenspace_rect) const
{
	GenericESP::UnionedRect unioned_rect{ screenspace_rect };

	if (box_enabled.get())
		box.draw(draw_list, player_pawn, unioned_rect);

	if (name_enabled.get()) {
		CSPlayerController* controller = player_pawn->original_controller().get();
		if (controller)
			name.draw(draw_list, player_pawn, controller->get_decorated_player_name(), unioned_rect);
	}

	if (healthbar_enabled.get())
		healthbar.draw(draw_list, player_pawn, unioned_rect);

	if (this->skeleton_enabled.get())
		draw_skeleton(player_pawn, draw_list, skeleton_line);
}

ESP::Player& ESP::get_player_by_pawn(CSPlayerPawn* player_pawn)
{
	if (player_pawn == Memory::local_player)
		return local;

	const TeamID local_team = Memory::local_player->team_id();
	const TeamID other_team = player_pawn->team_id();

	if (local_team == other_team)
		return teammates;
	if (local_team != other_team)
		return enemies;

	std::unreachable();
}

void ESP::update_camera_position(const glm::vec3& new_camera_position)
{
	camera_position.store(new_camera_position, std::memory_order::relaxed);
}

void ESP::draw(ImDrawList* draw_list)
{
	if (!enabled.get())
		return;

	if (!Memory::local_player)
		return;

	std::multiset<ESPEntity, ESPEntity::DistanceComparator> esp_entities;
	const glm::vec3 camera_position = this->camera_position.load(std::memory_order::relaxed);

	for (CSPlayerPawn* player_pawn : GameEntitySystem::the()->entities_of_type<CSPlayerPawn>()) {
		if (player_pawn->health() <= 0 || player_pawn->life_state() != LIFE_ALIVE)
			continue;

		// We don't want to render this one? Sure, then don't even care about it.
		if (!get_player_by_pawn(player_pawn).is_enabled())
			continue;

		GameSceneNode* game_scene_node = player_pawn->gameSceneNode();
		if (!game_scene_node)
			continue;

		CollisionProperty* collision = player_pawn->collision();
		if (!collision)
			continue;

		const glm::vec3 vec = game_scene_node->transform().m_Position;

		const glm::vec3 mins = collision->mins();
		const glm::vec3 maxs = collision->maxs();

		const glm::vec3 final_mins = vec + mins;
		const glm::vec3 final_maxs = vec + maxs;

#ifdef __cpp_lib_ranges_cartesian_product
		const auto points
			= std::views::cartesian_product(
				  std::array{ final_mins.x, final_maxs.x },
				  std::array{ final_mins.y, final_maxs.y },
				  std::array{ final_mins.z, final_maxs.z })
			| std::ranges::views::transform([](const auto& tuple) {
				  return glm::vec3{
					  std::get<0>(tuple),
					  std::get<1>(tuple),
					  std::get<2>(tuple),
				  };
			  });
#else
		const std::array points{
			// Lower
			glm::vec3{ final_mins[0], final_mins[1], final_mins[2] },
			glm::vec3{ final_maxs[0], final_mins[1], final_mins[2] },
			glm::vec3{ final_maxs[0], final_mins[1], final_maxs[2] },
			glm::vec3{ final_mins[0], final_mins[1], final_maxs[2] },
			// Higher
			glm::vec3{ final_mins[0], final_maxs[1], final_mins[2] },
			glm::vec3{ final_maxs[0], final_maxs[1], final_mins[2] },
			glm::vec3{ final_maxs[0], final_maxs[1], final_maxs[2] },
			glm::vec3{ final_mins[0], final_maxs[1], final_maxs[2] }
		};
#endif

		ImRect rectangle{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
			std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

		for (const glm::vec3& point : points) {
			ImVec2 point_2d;

			if (!Projection::project(point, point_2d)) {
				goto next_ent;
			}

			rectangle.Min.x = std::min(point_2d.x, rectangle.Min.x);
			rectangle.Min.y = std::min(point_2d.y, rectangle.Min.y);

			rectangle.Max.x = std::max(point_2d.x, rectangle.Max.x);
			rectangle.Max.y = std::max(point_2d.y, rectangle.Max.y);
		}

		esp_entities.emplace(
			player_pawn,
			// NOLINTNEXTLINE(readability-static-accessed-through-instance) -- TODO more entities
			player_pawn->classInfo(),
			glm::distance(camera_position, vec),
			rectangle);

	next_ent:;
	}

	for (const ESPEntity& esp_entity : esp_entities) {

		if (esp_entity.class_info == CSPlayerPawn::classInfo()) {
			auto* player_pawn = static_cast<CSPlayerPawn*>(esp_entity.entity);
			const Player& player_type = get_player_by_pawn(player_pawn);
			player_type.draw_player(draw_list, player_pawn, esp_entity.screenspace_rect);
			continue;
		}
	}
}
