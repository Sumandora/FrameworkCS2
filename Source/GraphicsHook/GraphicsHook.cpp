#include "GraphicsHook.hpp"

#include "../Memory.hpp"
#include "../SDK/Entities/BaseEntity.hpp"
#include "../SDK/Entities/GameEntitySystem.hpp"
#include "../SDK/GameClass/CollisionProperty.hpp"
#include "../SDK/GameClass/CSPlayerPawn.hpp"
#include "../SDK/GameClass/GameSceneNode.hpp"
#include "../Utils/Projection.hpp"
#include "imgui.h"

void drawViewMatrix()
{
	if (ImGui::Begin("View Matrix")) {
		if (ImGui::BeginTable("View Matrix", 4)) {
			for (std::array<float, 4>& row : *Memory::worldToProjectionMatrix) {
				for (float cell : row) {
					ImGui::TableNextColumn();
					ImGui::Text("%f", cell);
				}
			}
			ImGui::EndTable();
		}

		ImGui::End();
	}
}

void drawEntityList()
{
	int highest = (*Memory::EntitySystem::gameEntitySystem)->getHighestEntityIndex();
	if (ImGui::Begin("Entities")) {
		ImGui::Text("%d entities", highest);
		static bool onlyPlayers = false;
		ImGui::Checkbox("Only players", &onlyPlayers);
		if (highest > -1)
			if (ImGui::BeginTable("Entity List", 4, ImGuiTableFlags_SizingStretchProp)) {
				for (int i = 0; i <= highest; i++) {
					BaseEntity* entity = (*Memory::EntitySystem::gameEntitySystem)->getBaseEntity(i);
					if (entity == nullptr) {
						if (!onlyPlayers) {
							ImGui::TableNextColumn();
							ImGui::Text("null entity?");
							ImGui::TableNextColumn();
							ImGui::TableNextColumn();
							ImGui::TableNextColumn();
						}
						continue;
					}
					auto schemaType = entity->getSchemaType();
					if (onlyPlayers && schemaType != CSPlayerPawn::classInfo())
						continue;
					ImGui::TableNextColumn();
					ImGui::Text("%d", i);
					ImGui::TableNextColumn();
					ImGui::Text("%p", entity);
					ImGui::TableNextColumn();
					ImGui::Text("%s", schemaType->className);
					ImGui::TableNextColumn();
					auto* gameSceneNode = entity->gameSceneNode();
					auto* transform = gameSceneNode->transformPtr();
					if (transform == nullptr)
						continue;
					Vector3 vec = transform->m_Position;

					auto* collision = entity->collision();
					if (collision == nullptr)
						continue;
					Vector3 mins = collision->mins();
					Vector3 maxs = collision->maxs();

					Vector3 finalMins = { vec[0] + mins[0], vec[1] + mins[1], vec[2] + mins[2] };
					Vector3 finalMaxs = { vec[0] + maxs[0], vec[1] + maxs[1], vec[2] + maxs[2] };

					ImGui::Text("(%f %f %f) - (%f %f %f)",
						finalMins[0], finalMins[1], finalMins[2],
						finalMaxs[0], finalMaxs[1], finalMaxs[2]);
				}

				ImGui::EndTable();
			}

		ImGui::End();
	}
}

void drawEsp()
{
	int highest = (*Memory::EntitySystem::gameEntitySystem)->getHighestEntityIndex();
	if (highest > -1)
		for (int i = 0; i <= highest; i++) {
			BaseEntity* entity = (*Memory::EntitySystem::gameEntitySystem)->getBaseEntity(i);
			if (entity == nullptr)
				continue;
			auto schemaType = entity->getSchemaType();
			if (schemaType != CSPlayerPawn::classInfo())
				continue;

			auto* gameSceneNode = entity->gameSceneNode();
			auto* transform = gameSceneNode->transformPtr();
			if (transform == nullptr)
				continue;
			Vector3 vec = transform->m_Position;

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

			Vector4 rectangle{ FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX };

			for (const auto& point : points) {
				ImVec2 point2D;

				if (!project(point, point2D)) {
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

			ImGui::GetBackgroundDrawList()->AddRect(ImVec2{ rectangle[0], rectangle[1] }, ImVec2{ rectangle[2], rectangle[3] }, ImColor(255, 255, 255, 255), 2.0f, 0, 5.0f);
		next_ent:
		}
}

void GraphicsHook::mainLoop()
{
	drawViewMatrix();
	drawEntityList();
	drawEsp();
}
