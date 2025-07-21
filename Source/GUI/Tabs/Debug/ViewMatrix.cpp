#include "../Tabs.hpp"

#include "imgui.h"

#include "../../../Utils/Projection.hpp"

void GUI::Tabs::Debug::draw_view_matrix()
{
	if (ImGui::BeginTable("View Matrix", 4)) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				ImGui::TableNextColumn();
				ImGui::Text("%f", (*Projection::get_world_to_projection_matrix())[i][j]);
			}
		}
		ImGui::EndTable();
	}
}
