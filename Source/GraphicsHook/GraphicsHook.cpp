#include "GraphicsHook.hpp"

#include "../Memory.hpp"
#include "../SDK/Entities/BaseEntity.hpp"
#include "../SDK/Entities/CSPlayerPawn.hpp"
#include "../SDK/Entities/GameEntitySystem.hpp"
#include "../SDK/GameClass/CollisionProperty.hpp"
#include "../SDK/GameClass/GameSceneNode.hpp"

#include "../SDK/ConVar/ConVar.hpp"
#include "../SDK/ConVar/EngineCvar.hpp"

#include "../Features/Features.hpp"
#include "BCRL/Session.hpp"
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
	}
	ImGui::End();
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
	}
	ImGui::End();
}

void drawConVars() {
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
				if(BCRL::SafePointer(Memory::mem_mgr, (std::uintptr_t)convar->value.string).is_valid() /*:thumbsup:*/)
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
}

void drawLocalPlayer() {
	if (ImGui::Begin("Local player")) {
		auto localPlayer = BaseEntity::getLocalPlayer();
		if (localPlayer != nullptr)
			ImGui::Text("Local player: %p", localPlayer);
		else
			ImGui::Text("No local player found!");
	}
	ImGui::End();
}

extern std::unordered_map<std::string, std::size_t> eventCounters;

void drawEventList() {
	if(ImGui::Begin("Game events")) {
		if (ImGui::BeginTable("Events", 2)) {
			for(const auto& [eventName, count] : eventCounters) {
				ImGui::TableNextColumn();
				ImGui::Text("%s", eventName.c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%lu", count);
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void GraphicsHook::mainLoop()
{
	drawViewMatrix();
	drawEntityList();
	drawConVars();
	drawLocalPlayer();
	drawEventList();

	Features::ESP::imguiRender();
	Features::ForceCrosshair::imguiRender();
}
