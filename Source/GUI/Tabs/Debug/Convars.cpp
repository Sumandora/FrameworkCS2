#include "../Tabs.hpp"

#include "../../../Interfaces.hpp"
#include "../../../Memory.hpp"
#include "../../../SDK/ConVar/ConVar.hpp"
#include "../../../SDK/ConVar/EngineCvar.hpp"

#include "BCRL/SafePointer.hpp"

#include "imgui.h"

void GUI::Tabs::Debug::draw_convars()
{
	static char search[128] = "";
	ImGui::InputText("Search", search, IM_ARRAYSIZE(search));
	static bool searchBackend = false;
	ImGui::Checkbox("Search backend", &searchBackend);

	if (ImGui::Button("Remove dev protection")) {
		for (int i = 0; i < Interfaces::engineCvar->convar_count; i++) {
			auto& listElem = Interfaces::engineCvar->convarList.memory.memory[i];
			if (listElem.element)
				listElem.element->flags.set(1, false);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove hidden state")) {
		for (int i = 0; i < Interfaces::engineCvar->convar_count; i++) {
			auto& listElem = Interfaces::engineCvar->convarList.memory.memory[i];
			if (listElem.element)
				listElem.element->flags.set(4, false);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove cheat protection")) {
		for (int i = 0; i < Interfaces::engineCvar->convar_count; i++) {
			auto& listElem = Interfaces::engineCvar->convarList.memory.memory[i];
			if (listElem.element)
				listElem.element->flags.set(14, false);
		}
	}

	if (ImGui::BeginTable("List", 6)) {
		auto process = [](ConVar* convar) {
			if (convar == nullptr)
				return false;

			if (search[0] != '\0' && strncmp(convar->name, search, strlen(search)) != 0)
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
			if (BCRL::SafePointer(Memory::mem_mgr, (std::uintptr_t)convar->value.string).is_valid() /*:thumbsup:*/)
				ImGui::Text("%s", convar->value.string);
			else
				ImGui::Text("null");
			ImGui::TableNextColumn();
			ImGui::Text("%d", convar->value.integer);
			ImGui::TableNextColumn();
			ImGui::Text("%f", convar->value.floatingPoint);

			return true;
		};

		if (searchBackend) {
			for (int i = 0; i < Interfaces::engineCvar->convar_count; i++) {
				auto& listElem = Interfaces::engineCvar->convarList.memory.memory[i];
				if (!process(listElem.element))
					break;
			}
		} else {
			for (auto it = Interfaces::engineCvar->getFirstCvarIterator(); it; it = Interfaces::engineCvar->getNextCvarIterator(it)) {
				auto* convar = Interfaces::engineCvar->getCvar(it);
				if (!process(convar))
					break;
			}
		}
		ImGui::EndTable();
	}
}
