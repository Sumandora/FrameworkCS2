#include <thread>

#include "Interfaces.hpp"

#include "Memory.hpp"

#include "GraphicsHook/GraphicsHook.hpp"

#include "imgui.h"
#include "SDK/Schema/SchemaClassInfo.hpp"
#include "SDK/Schema/SchemaSystem.hpp"
#include "SDK/Schema/SchemaSystemTypeScope.hpp"
#include "SDK/Schema/Types/FieldData.hpp"
#include "SDK/Schema/Types/SchemaType.hpp"
#include "SDK/Schema/Types/StaticFieldData.hpp"

#include "SDK/Entities/BaseEntity.hpp"
#include "SDK/Entities/GameEntitySystem.hpp"

#include "SDK/GameClass/CollisionProperty.hpp"
#include "SDK/GameClass/CSPlayerPawn.hpp"
#include "SDK/GameClass/CGameSceneNode.hpp"
#include "SDK/GameClass/CTransform.hpp"

#include "SDK/Math/Vector.hpp"

void printFields(SchemaClassInfo* classInfo)
{
	printf("Regular fields:\n");
	for (std::size_t i = 0; i < classInfo->fieldsCount; i++) {
		FieldData* field = &classInfo->fields[i];
		printf("%s at %x with type %s\n", field->fieldName, field->offset, field->type->name);
	}

	printf("Static fields:\n");
	for (std::size_t i = 0; i < classInfo->staticFieldsCount; i++) {
		StaticFieldData* field = &classInfo->staticFields[i];
		printf("%s is type of %s and is located at %p\n", field->fieldName, field->type->name, field->data);
	}
}

void initializer()
{
	printf("Hello, world, again!\n");

	Interfaces::getInterfaces();

	Memory::Create();

	printFields(CSPlayerPawn::classInfo());

	if (!GraphicsHook::hookSDL()) {
		printf("Failed to hook SDL\n");
		return;
	}

	if (!GraphicsHook::hookVulkan()) {
		printf("Failed to hook vulkan\n");
		return;
	}

	GraphicsHook::mainLoop = [&]() {
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
						if (strcmp(CSPlayerPawn::classInfo()->className, schemaType->className) == 0) {
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
					}

					ImGui::EndTable();
				}

			ImGui::End();
		}
	};
}

int __attribute((constructor, used)) startup()
{
	printf("Hello, world!\n");
	std::thread t(initializer);
	t.detach();

	return 0;
}

void __attribute((destructor)) shutdown()
{
	GraphicsHook::unhookSDL();
	GraphicsHook::unhookVulkan();
}
