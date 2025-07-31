#include "../Tabs.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/Entities/GameEntitySystem.hpp"

#include "imgui.h"

void GUI::Tabs::Debug::draw_entity_list()
{
	static bool only_players = false;
	ImGui::Checkbox("Only players", &only_players);

	int count = 0;

	if (ImGui::BeginTable(
			"Entity List",
			4 /*This should be 2, but having 2 columns of entities is much more readable*/,
			ImGuiTableFlags_SizingStretchProp)) {
		if (only_players)
			for (CSPlayerPawn* entity : GameEntitySystem::the()->entities_of_type<CSPlayerPawn>()) {
				ImGui::TableNextColumn();
				ImGui::Text("%p", static_cast<void*>(entity));
				ImGui::TableNextColumn();
				ImGui::Text("%s", entity->getSchemaType()->className);
				count++;
			}
		else
			for (BaseEntity* entity : GameEntitySystem::the()->entities()) {
				ImGui::TableNextColumn();
				ImGui::Text("%p", static_cast<void*>(entity));
				ImGui::TableNextColumn();
				ImGui::Text("%s", entity->getSchemaType()->className);
				count++;
			}

		ImGui::EndTable();
	}

	ImGui::Text("Actual count: %d", count);
}
