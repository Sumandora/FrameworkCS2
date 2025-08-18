#include "ESP.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <set>
#include <string>
#include <utility>

#include "../../../Memory.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/Entities/GameEntitySystem.hpp"
#include "../../../SDK/Enums/LifeState.hpp"
#include "../../../SDK/Enums/TeamID.hpp"
#include "../../../SDK/GameClass/CollisionProperty.hpp"
#include "../../../SDK/GameClass/GameSceneNode.hpp"
#include "../../../SDK/Schema/FieldOffset.hpp"
#include "../../../SDK/Schema/SchemaClassInfo.hpp"

#include "../../../Utils/BulletSimulation.hpp"
#include "../../../Utils/Projection.hpp"

#include "../../Feature.hpp"

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "Player.hpp"

ESP::ESP()
	: Feature("Visuals", "ESP")
{
}

ESPPlayer& ESP::get_player_by_pawn(CSPlayerPawn* player_pawn)
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

enum class EntityType : std::int8_t {
	UNK = -1,
	PLAYER,
	WEAPON
};

struct ESPEntity {
	BaseEntity* entity;
	EntityType type;
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

void ESP::draw(ImDrawList* draw_list)
{
	if (!enabled.get())
		return;

	if (!Memory::local_player)
		return;

	std::multiset<ESPEntity, ESPEntity::DistanceComparator> esp_entities;
	const glm::vec3 camera_position = this->camera_position.load(std::memory_order::relaxed);

	for (BaseEntity* entity : GameEntitySystem::the()->entities()) {
		SchemaClassInfo* class_info = entity->getSchemaType();

		EntityType type = EntityType::UNK;
		if (class_info == CSPlayerPawn::classInfo()) {
			if (entity->health() <= 0 || entity->life_state() != LIFE_ALIVE)
				continue;

			// We don't want to render this one? Sure, then don't even care about it.
			if (!get_player_by_pawn(static_cast<CSPlayerPawn*>(entity)).is_enabled())
				continue;

			type = EntityType::PLAYER;
		} else if (auto* weapon = schema_cast<BasePlayerWeapon*>(entity, class_info); weapon) {
			if (weapon->owner_entity().has_entity())
				continue;

			if (!weapons.is_enabled())
				continue;

			type = EntityType::WEAPON;
		} else
			continue;

		GameSceneNode* game_scene_node = entity->gameSceneNode();
		if (!game_scene_node)
			continue;

		CollisionProperty* collision = entity->collision();
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
			entity,
			type,
			glm::distance(camera_position, vec),
			rectangle);

	next_ent:;
	}

	for (const ESPEntity& esp_entity : esp_entities) {
		switch (esp_entity.type) {
		case EntityType::PLAYER: {
			auto* player_pawn = static_cast<CSPlayerPawn*>(esp_entity.entity);
			const ESPPlayer& player_type = get_player_by_pawn(player_pawn);
			player_type.draw_player(draw_list, player_pawn, esp_entity.screenspace_rect);
			continue;
		}
		case EntityType::WEAPON: {
			auto* weapon = static_cast<BasePlayerWeapon*>(esp_entity.entity);
			weapons.draw_weapon(draw_list, weapon, esp_entity.screenspace_rect);
			break;
		}
		default:
			std::unreachable();
		}
	}
}
