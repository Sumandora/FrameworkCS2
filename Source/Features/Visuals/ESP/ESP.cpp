#include "ESP.hpp"

#include <concepts>
#include <functional>
#include <limits>
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

struct SkeletonTree {
	int index;
	std::vector<SkeletonTree> subtrees;

	template <typename... T>
		requires(std::same_as<T, SkeletonTree> && ...)
	explicit SkeletonTree(int index, T... subtress)
		: index(index)
		, subtrees{ std::forward<T>(subtress)... }
	{
	}
};

// TODO Flatten
const static SkeletonTree SKELETON_HEAD{
	6,
	SkeletonTree{
		5,
		// Arms
		SkeletonTree{ 13, SkeletonTree{ 14, SkeletonTree{ 15 } } },
		SkeletonTree{ 8, SkeletonTree{ 9, SkeletonTree{ 10 } } },

		SkeletonTree{
			4,
			SkeletonTree{
				3,
				SkeletonTree{
					2,
					SkeletonTree{
						1,
						SkeletonTree{
							0,
							// Legs
							SkeletonTree{ 25, SkeletonTree{ 26, SkeletonTree{ 27 } } },
							SkeletonTree{ 22, SkeletonTree{ 23, SkeletonTree{ 24 } } },
						},
					},
				},
			},
		},
	},
};

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

			glm::vec3 final_mins = { vec[0] + mins[0], vec[1] + mins[1], vec[2] + mins[2] };
			glm::vec3 final_maxs = { vec[0] + maxs[0], vec[1] + maxs[1], vec[2] + maxs[2] };

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
						name.draw(draw_list, entity, controller->sanitized_name(), unioned_rect);
				}
				if (healthbar_enabled.get())
					healthbar.draw(draw_list, entity, unioned_rect);
			}

			// TODO there can be flickers before the player is respawned, I'm not sure how to approach this quite yet.
			if (this->skeleton.get()) {
				BodyComponent* body_component = entity->body_component();

				const SkeletonInstance& skeleton = static_cast<BodyComponentSkeletonInstance*>(body_component)->skeleton_instance();

				if (skeleton.bone_count == 0)
					continue;

				static const std::function<void(const SkeletonInstance&, const SkeletonTree&)> PROCESS_TREE
					= [&](const SkeletonInstance& skeleton, const SkeletonTree& tree) {
						  bool can_draw = true;
						  ImVec2 from_screen_space;
						  if (!Projection::project(skeleton.bones[tree.index].pos, from_screen_space))
							  can_draw = false;

						  for (const SkeletonTree& subtree : tree.subtrees) {
							  if (can_draw) {
								  ImVec2 to_screen_space;
								  if (Projection::project(skeleton.bones[subtree.index].pos, to_screen_space)) {
									  skeleton_line.draw(draw_list, entity, from_screen_space, to_screen_space);
								  }
							  }

							  PROCESS_TREE(skeleton, subtree);
						  }
					  };

				PROCESS_TREE(skeleton, SKELETON_HEAD);
			}
		next_ent:;
		}
}
