#include "ESP.hpp"

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <limits>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "../../../Memory.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/Entities/Components/BodyComponentSkeletonInstance.hpp"
#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/Entities/GameEntitySystem.hpp"
#include "../../../SDK/Enums/LifeState.hpp"
#include "../../../SDK/GameClass/CollisionProperty.hpp"
#include "../../../SDK/GameClass/GameSceneNode.hpp"
#include "../../../SDK/GameClass/SkeletonInstance.hpp"

#include "../../../Utils/Projection.hpp"

#include "../../Feature.hpp"

#include "GenericESP/UnionedRect.hpp"

#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

#include "imgui.h"
#include "imgui_internal.h"

ESP::ESP()
	: Feature("Visuals", "ESP")
{
}

// TODO Are hardcoded IDs safe here?
static constexpr std::initializer_list<std::pair<int, int>> SKELETON_LINES{
	{ 6, 5 },

	// Arms
	{ 5, 13 },
	{ 13, 14 },
	{ 14, 15 },

	{ 5, 8 },
	{ 8, 9 },
	{ 9, 10 },

	// Torso
	{ 5, 4 },
	{ 4, 3 },
	{ 3, 2 },
	{ 2, 1 },
	{ 1, 0 },

	// Legs
	{ 0, 25 },
	{ 25, 26 },
	{ 26, 27 },

	{ 0, 22 },
	{ 22, 23 },
	{ 23, 24 }
};
// TODO This would be prettier with std::ranges::views::concat, but its c++26
static constexpr std::size_t HIGHEST_SKELETON_INDEX
	= std::max(
		std::ranges::max(SKELETON_LINES | std::ranges::views::keys),
		std::ranges::max(SKELETON_LINES | std::ranges::views::values));

void ESP::draw(ImDrawList* draw_list)
{
	if (!enabled.get())
		return;
	const int highest = GameEntitySystem::the()->getHighestEntityIndex();
	if (highest > -1)
		for (int i = 0; i <= highest; i++) {
			BaseEntity* entity = GameEntitySystem::the()->getBaseEntity(i);
			if (entity == nullptr)
				continue;
			auto* player_pawn = entity->entity_cast<CSPlayerPawn*>();
			if (!player_pawn)
				continue;

			if (player_pawn == Memory::local_player)
				continue;

			if (player_pawn->health() <= 0 || player_pawn->life_state() != LIFE_ALIVE)
				continue;

			GameSceneNode* game_scene_node = entity->gameSceneNode();
			auto& transform = game_scene_node->transform();
			glm::vec3 vec = transform.m_Position;

			CollisionProperty* collision = entity->collision();
			if (collision == nullptr)
				continue;
			glm::vec3 mins = collision->mins();
			glm::vec3 maxs = collision->maxs();

			// TODO Vector arithmetic
			glm::vec3 final_mins = { vec[0] + mins[0], vec[1] + mins[1], vec[2] + mins[2] };
			glm::vec3 final_maxs = { vec[0] + maxs[0], vec[1] + maxs[1], vec[2] + maxs[2] };

			// TODO This is just cartesian product
			const glm::vec3 points[] = {
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

			glm::vec4 rectangle{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
				std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

			for (const auto& point : points) {
				ImVec2 point_2d;

				if (!Projection::project(point, point_2d)) {
					goto next_ent;
				}

				if (point_2d.x < rectangle[0])
					rectangle[0] = point_2d.x;
				else if (point_2d.x > rectangle[2])
					rectangle[2] = point_2d.x;

				if (point_2d.y < rectangle[1])
					rectangle[1] = point_2d.y;
				else if (point_2d.y > rectangle[3])
					rectangle[3] = point_2d.y;
			}

			{
				GenericESP::UnionedRect unioned_rect{ ImRect{ rectangle[0], rectangle[1], rectangle[2], rectangle[3] } };
				if (box_enabled.get())
					box.draw(draw_list, entity, unioned_rect);
				if (name_enabled.get()) {
					CSPlayerController* controller = player_pawn->original_controller().get();
					if (controller)
						name.draw(draw_list, entity, controller->get_decorated_player_name(), unioned_rect);
				}
				if (healthbar_enabled.get())
					healthbar.draw(draw_list, entity, unioned_rect);
			}

			// TODO The shadows should be drawn first and then have a separate pass adding the actual lines, so that the shadows are actually behind the lines
			// -> Fix in GenericESP
			if (this->skeleton.get()) {
				BodyComponent* body_component = entity->body_component();

				const SkeletonInstance& skeleton = static_cast<BodyComponentSkeletonInstance*>(body_component)->skeleton_instance();

				if (skeleton.bone_count == 0)
					continue;

				// The following code mainly tries to avoid projecting points multiple times.

				// Mind that there are gaps in the skeleton lines indexes, so these can't just be computed upfront
				// So this kind of lazy system is used instead
				struct ScreenSpacePoint {
					ImVec2 coords;
					bool visible : 1 = false;
					bool looked_up : 1 = false;

					void get(glm::vec3 world_space) &
					{
						visible = Projection::project(world_space, coords);
					}
				};
				// Yes, there are holes and yes this is allocating more stack memory than necessary, but it's faster
				std::array<ScreenSpacePoint, HIGHEST_SKELETON_INDEX + 1 /* 0 is also a valid index */> screen_space_vecs{};

				for (const auto& [from, to] : SKELETON_LINES) {
					ScreenSpacePoint& from_screen_space = screen_space_vecs[from];
					if (!from_screen_space.looked_up)
						from_screen_space.get(skeleton.bones[from].pos);
					if (!from_screen_space.visible)
						continue;

					ScreenSpacePoint& to_screen_space = screen_space_vecs[to];
					if (!to_screen_space.looked_up)
						to_screen_space.get(skeleton.bones[to].pos);
					if (!to_screen_space.visible)
						continue;

					skeleton_line.draw(draw_list, entity, from_screen_space.coords, to_screen_space.coords);
				}
			}
		next_ent:;
		}
}
