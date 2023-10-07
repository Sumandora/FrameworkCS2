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

#include "SDK/Math/Vector.hpp"

#include "SDK/GameClass/EngineCvar.hpp"
#include "SDK/GameClass/ConVar.hpp"

#include "BCRL.hpp"

SchemaClassInfo* gameSceneNode;
SchemaClassInfo* csPlayerPawn;
SchemaClassInfo* baseEntity;
SchemaClassInfo* collisionProperty;

int gameSceneNodeOffset;
int transformOffset;
int collisionOffset;

int vecMins;
int vecMaxs;

void initializer()
{
	printf("Hello, world, again!\n");

	Interfaces::getInterfaces();

	Memory::Create();

	csPlayerPawn = Interfaces::schemaSystem->FindTypeScopeForModule("libclient.so")->FindDeclaredClass("C_CSPlayerPawn");
	baseEntity = Interfaces::schemaSystem->FindTypeScopeForModule("libclient.so")->FindDeclaredClass("C_BaseEntity");
	collisionProperty = Interfaces::schemaSystem->FindTypeScopeForModule("libclient.so")->FindDeclaredClass("CCollisionProperty");
	gameSceneNode = Interfaces::schemaSystem->FindTypeScopeForModule("libclient.so")->FindDeclaredClass("CGameSceneNode");

	printf("CSPlayerPawn: %p\nBaseEntity: %p\nCollisionProp: %p\n", csPlayerPawn, baseEntity, collisionProperty);

	auto findField = [](SchemaClassInfo* classInfo, const char* fieldName) {
		for (std::size_t i = 0; i < classInfo->fieldsCount; i++) {
			FieldData& field = classInfo->fields[i];
			if(strcmp(field.fieldName, fieldName) == 0)
				return field.offset;
		}
		return 0;
	};

	gameSceneNodeOffset = findField(baseEntity, "m_pGameSceneNode");
	transformOffset = findField(gameSceneNode, "m_nodeToWorld");
	collisionOffset = findField(baseEntity, "m_pCollision");

	vecMins = findField(collisionProperty, "m_vecMins");
	vecMaxs = findField(collisionProperty, "m_vecMaxs");

	printf("mins: %x\nmaxs: %x\n", vecMins, vecMaxs);

	for (std::size_t i = 0; i < csPlayerPawn->fieldsCount; i++) {
		FieldData* field = &csPlayerPawn->fields[i];
		printf("%s at %x with type %s\n", field->fieldName, field->offset, field->type->name);
	}

	for (std::size_t i = 0; i < csPlayerPawn->staticFieldsCount; i++) {
		StaticFieldData* field = &csPlayerPawn->staticFields[i];
		printf("%s is type of %s and is located at %p\n", field->fieldName, field->type->name, field->data);
	}

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

		}
		ImGui::End();

		int highest = (*Memory::EntitySystem::gameEntitySystem)->getHighestEntityIndex();
		if (ImGui::Begin("Entities")) {
			ImGui::Text("%d entities", highest);
			static bool onlyPlayers = false;
			ImGui::Checkbox("Only players", &onlyPlayers);
			if (highest > -1)
				if (ImGui::BeginTable("Entity List", 4)) {
					for (int i = 0; i <= highest; i++) {
						BaseEntity* entity = (*Memory::EntitySystem::gameEntitySystem)->getBaseEntity(i);
						if (entity == nullptr) {
							if(!onlyPlayers) {
								ImGui::TableNextColumn();
								ImGui::Text("null entity?");
								ImGui::TableNextColumn();
								ImGui::TableNextColumn();
								ImGui::TableNextColumn();
							}
							continue;
						}
						auto schemaType = entity->getSchemaType();
						if(onlyPlayers && schemaType != csPlayerPawn)
							continue;
						ImGui::TableNextColumn();
						ImGui::Text("%d", i);
						ImGui::TableNextColumn();
						ImGui::Text("%p", entity);
						ImGui::TableNextColumn();
						ImGui::Text("%s", schemaType->className);
						ImGui::TableNextColumn();
						if (strcmp(csPlayerPawn->className, schemaType->className) == 0) {
							char* gameSceneNode = *(char**)((char*)entity + gameSceneNodeOffset);
							class alignas(16) CTransform {
							public:
								alignas(16) Vector3 m_Position;
							};
							auto* transform = (CTransform*)(gameSceneNode + transformOffset);
							if(transform == nullptr)
								continue;
							Vector3 vec = transform->m_Position;


							char* collision = *(char**)((char*)entity + collisionOffset);
							Vector3 mins = *(Vector3*)(collision + vecMins);
							Vector3 maxs = *(Vector3*)(collision + vecMaxs);


							Vector3 finalMins = { vec[0] + mins[0], vec[1] + mins[1], vec[2] + mins[2] };
							Vector3 finalMaxs = { vec[0] + maxs[0], vec[1] + maxs[1], vec[2] + maxs[2] };

							ImGui::Text("(%f %f %f) - (%f %f %f)",
								finalMins[0], finalMins[1], finalMins[2],
								finalMaxs[0], finalMaxs[1], finalMaxs[2]);
						}
					}

					ImGui::EndTable();
				}

		}
		ImGui::End();

		if(ImGui::Begin("Convars")) {
			static char search[128] = "";
			ImGui::InputText("Search", search, IM_ARRAYSIZE(search));
			static bool searchBackend = false;
			ImGui::Checkbox("Search backend", &searchBackend);

			if(ImGui::Button("Remove dev protection")) {
				auto& list = Interfaces::engineCvar->convarList;
				auto it = list.head;
				while (it != list.INVALID_INDEX) {
					auto& listElem = list.memory.memory[it];
					listElem.element->flags.set(1, false);

					it = listElem.next;
				}
			}

			ImGui::SameLine();

			if(ImGui::Button("Remove hidden state")) {
				auto& list = Interfaces::engineCvar->convarList;
				auto it = list.head;
				while (it != list.INVALID_INDEX) {
					auto& listElem = list.memory.memory[it];
					listElem.element->flags.set(4, false);

					it = listElem.next;
				}
			}

			ImGui::SameLine();

			if(ImGui::Button("Remove cheat protection")) {
				auto& list = Interfaces::engineCvar->convarList;
				auto it = list.head;
				while (it != list.INVALID_INDEX) {
					auto& listElem = list.memory.memory[it];
					listElem.element->flags.set(14, false);

					it = listElem.next;
				}
			}

			if (ImGui::BeginTable("List", 6)) {
				auto process = [](ConVar* convar) {
					if(convar == nullptr)
						return false;

					if(search[0] != '\0' && strncmp(convar->name, search, strlen(search)) != 0)
						return true;

					ImGui::TableNextColumn();
					ImGui::Text("%s", convar->name);
					ImGui::TableNextColumn();
					ImGui::Text("%s", convar->description);
					ImGui::TableNextColumn();
					auto bitsetToString = []<std::size_t N>(std::bitset<N> bitset) {
						std::string s;
						for (size_t i = 0; i < bitset.size(); i++) {
							s += bitset[i] ? '1' : '0';
						}
						return s;
					};
					ImGui::Text("%s", bitsetToString(convar->flags).c_str());
					ImGui::TableNextColumn();
					if(BCRL::SafePointer(convar->value.string).isValid() /*:thumbsup:*/)
						ImGui::Text("%s", convar->value.string);
					else
						ImGui::Text("null");
					ImGui::TableNextColumn();
					ImGui::Text("%d", convar->value.integer);
					ImGui::TableNextColumn();
					ImGui::Text("%f", convar->value.floatingPoint);

					return true;
				};

				if(searchBackend) {
					auto& list = Interfaces::engineCvar->convarList;
					auto it = list.head;
					while (it != list.INVALID_INDEX) {
						auto& listElem = list.memory.memory[it];
						if(!process(listElem.element))
							break;

						it = listElem.next;
					}
				} else {
					for(auto it = Interfaces::engineCvar->getFirstCvarIterator(); it; it = Interfaces::engineCvar->getNextCvarIterator(it)) {
						auto* convar = Interfaces::engineCvar->getCvar(it);
						if(!process(convar))
							break;
					}
				}
				ImGui::EndTable();
			}

		}
		ImGui::End();
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
