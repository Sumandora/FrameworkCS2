#include "GraphicsHook.hpp"

#include "../Memory.hpp"
#include "../SDK/Entities/BaseEntity.hpp"
#include "../SDK/Entities/GameEntitySystem.hpp"
#include "../SDK/GameClass/CollisionProperty.hpp"
#include "../SDK/GameClass/CSPlayerPawn.hpp"
#include "../SDK/GameClass/GameSceneNode.hpp"
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

void GraphicsHook::mainLoop()
{
	drawViewMatrix();
	drawEntityList();
}
