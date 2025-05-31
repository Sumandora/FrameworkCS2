#include "Tabs.hpp"

#include "../../Features/Feature.hpp"
#include "../../Features/Features.hpp"

#include "../GUI.hpp"

#include "imgui.h"

#include <string>

void GUI::Tabs::render()
{
	ImGui::SetNextWindowSize(ImVec2{ 400, 300 } * GUI::get_scale(), ImGuiCond_Once);
	if (ImGui::Begin("Framework CS2")) {
		if (ImGui::BeginTabBar("#Top level tabs", ImGuiTabBarFlags_Reorderable)) {
			for (auto& [category, vector] : Features::features) {
				if (ImGui::BeginTabItem(category.c_str())) {
					std::string tag = category;
					tag.insert(0, 1, '#');
					if (ImGui::BeginTabBar(tag.c_str(), ImGuiTabBarFlags_Reorderable)) {
						for (Feature* feature : vector) {
							if (ImGui::BeginTabItem(feature->get_name().c_str())) {
								feature->render();
								ImGui::EndTabItem();
							}
						}
						ImGui::EndTabBar();
					}
					ImGui::EndTabItem();
				}
			}

			if (ImGui::BeginTabItem("Debug")) {
				if (ImGui::BeginTabBar("#Debug tabs", ImGuiTabBarFlags_Reorderable)) {
					// TODO Link maps/Shared libraries

					if(ImGui::BeginTabItem("Interfaces")) {
						Debug::draw_interfaces();
						ImGui::EndTabItem();
					}
					if(ImGui::BeginTabItem("Link maps")) {
						Debug::draw_link_maps();
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("View matrix")) {
						Debug::draw_view_matrix();
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Entities")) {
						Debug::draw_entity_list();
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Convars")) {
						Debug::draw_convars();
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Local player")) {
						Debug::draw_local_player();
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Game events")) {
						Debug::draw_event_list();
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}
