#include "../Tabs.hpp"

#include "../../../Memory.hpp"

#include "imgui.h"

#include <array>

void GUI::Tabs::Debug::draw_view_matrix()
{
	if (ImGui::BeginTable("View Matrix", 4)) {
		for (const std::array<float, 4>& row : *Memory::worldToProjectionMatrix) {
			for (const float cell : row) {
				ImGui::TableNextColumn();
				ImGui::Text("%f", cell);
			}
		}
		ImGui::EndTable();
	}
}
