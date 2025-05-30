#include "../Tabs.hpp"

#include "imgui.h"

#include "../../../Memory.hpp"

void GUI::Tabs::Debug::draw_view_matrix()
{
	if (ImGui::BeginTable("View Matrix", 4)) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				ImGui::TableNextColumn();
				ImGui::Text("%f", Memory::worldToProjectionMatrix[i][j]);
			}
		}
		ImGui::EndTable();
	}
}
