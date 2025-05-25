#include "ESP.hpp"

#include <limits>

#include "../../Memory.hpp"
#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/GameEntitySystem.hpp"

#include "../../SDK/GameClass/CollisionProperty.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"
#include "../../Utils/Projection.hpp"

#include "imgui.h"

ESP::ESP()
	: Feature("ESP")
{
}

void ESP::draw(ImDrawList* drawList)
{
	if (!enabled.get())
		return;
	int highest = GameEntitySystem::the()->getHighestEntityIndex();
	if (highest > -1)
		for (int i = 0; i <= highest; i++) {
			BaseEntity* entity = GameEntitySystem::the()->getBaseEntity(i);
			if (entity == nullptr)
				continue;
			auto schemaType = entity->getSchemaType();
			if (schemaType != CSPlayerPawn::classInfo())
				continue;

			auto* gameSceneNode = entity->gameSceneNode();
			auto& transform = gameSceneNode->transform();
			Vector3 vec = transform.m_Position;

			auto* collision = entity->collision();
			if (collision == nullptr)
				continue;
			Vector3 mins = collision->mins();
			Vector3 maxs = collision->maxs();

			Vector3 finalMins = { vec[0] + mins[0], vec[1] + mins[1], vec[2] + mins[2] };
			Vector3 finalMaxs = { vec[0] + maxs[0], vec[1] + maxs[1], vec[2] + maxs[2] };

			const Vector3 points[] = {
				// Lower
				Vector3{ finalMins[0], finalMins[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMins[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMins[1], finalMaxs[2] },
				Vector3{ finalMins[0], finalMins[1], finalMaxs[2] },
				// Higher
				Vector3{ finalMins[0], finalMaxs[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMaxs[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMaxs[1], finalMaxs[2] },
				Vector3{ finalMins[0], finalMaxs[1], finalMaxs[2] }
			};

			Vector4 rectangle{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
				std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

			for (const auto& point : points) {
				ImVec2 point2D;

				if (!Projection::project(point, point2D)) {
					goto next_ent;
				}

				if (point2D.x < rectangle[0])
					rectangle[0] = point2D.x;
				else if (point2D.x > rectangle[2])
					rectangle[2] = point2D.x;

				if (point2D.y < rectangle[1])
					rectangle[1] = point2D.y;
				else if (point2D.y > rectangle[3])
					rectangle[3] = point2D.y;
			}

			drawList->AddRect(ImVec2{ rectangle[0], rectangle[1] }, ImVec2{ rectangle[2], rectangle[3] }, ImColor(255, 255, 255, 255), 2.0f, 0, 5.0f);
		next_ent:;
		}
}
