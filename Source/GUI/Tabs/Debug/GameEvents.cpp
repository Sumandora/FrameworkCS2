#include "../Tabs.hpp"

#include "../../../Hooks/Game/GameHook.hpp"

#include "imgui.h"

void GUI::Tabs::Debug::draw_event_list()
{
	if (ImGui::BeginTable("Events", 2)) {
		for (const auto& [eventName, count] : Hooks::Game::FireEvent::event_counters) {
			ImGui::TableNextColumn();
			ImGui::Text("%s", eventName.c_str());
			ImGui::TableNextColumn();
			ImGui::Text("%lu", count);
		}
		ImGui::EndTable();
	}
}
