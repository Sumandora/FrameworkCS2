#include "ESP.hpp"

#include <limits>

#include "../../../Memory.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/Entities/GameEntitySystem.hpp"
#include "../../../SDK/GameClass/CollisionProperty.hpp"
#include "../../../SDK/GameClass/GameSceneNode.hpp"
#include "../../../SDK/Schema/SchemaClassInfo.hpp"

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
			SchemaClassInfo* schema_type = entity->getSchemaType();
			if (schema_type != CSPlayerPawn::classInfo())
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
				if (box.enabled.get())
					box.draw(draw_list, entity, unioned_rect);
				if (name.enabled.get())
					name.draw(draw_list, entity, "Hello, world", unioned_rect);
				if (healthbar.enabled.get())
					healthbar.draw(draw_list, entity, unioned_rect);
			}
		next_ent:;
		}
}
