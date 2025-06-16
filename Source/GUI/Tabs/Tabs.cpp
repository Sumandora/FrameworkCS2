#include "Tabs.hpp"

#include "../../Features/Feature.hpp"
#include "../../Features/Features.hpp"

#include "../Elements/BoxedTabItem.hpp"
#include "../GUI.hpp"

#include "imgui.h"

#include <ranges>
#include <string>

void GUI::Tabs::render()
{
	ImGui::SetNextWindowSizeConstraints(ImVec2{ 400, 300 } * GUI::get_scale(), ImVec2(FLT_MAX, FLT_MAX));
	ImGui::SetNextWindowSize(ImVec2{ 600, 450 } * GUI::get_scale(), ImGuiCond_Once);
	if (ImGui::Begin("Framework CS2", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar)) {
		const ImVec2 remaining_size{ 0.0F, -ImGui::GetFontSize() };

		if (ImGui::BeginTabBar("#Top level tabs", ImGuiTabBarFlags_Reorderable)) {
			for (auto& [category, vector] : Features::features) {
				if (ImGuiExt::BeginBoxedTabItem(category.c_str(), remaining_size)) {
					std::string tag = category;
					tag.insert(0, 1, '#');
					if (ImGui::BeginTabBar(tag.c_str(), ImGuiTabBarFlags_Reorderable)) {
						for (Feature* feature : vector) {
							if (ImGuiExt::BeginBoxedTabItem(feature->get_name().c_str(), ImGui::GetContentRegionAvail())) {
								feature->render_all_childs();
								ImGuiExt::EndBoxedTabItem();
							}
						}
						ImGui::EndTabBar();
					}
					ImGuiExt::EndBoxedTabItem();
				}
			}

			if (ImGuiExt::BeginBoxedTabItem("Debug", remaining_size)) {
				if (ImGui::BeginTabBar("#Debug tabs", ImGuiTabBarFlags_Reorderable)) {
					if (ImGuiExt::BeginBoxedTabItem("Interfaces", ImGui::GetContentRegionAvail())) {
						Debug::draw_interfaces();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Link maps", ImGui::GetContentRegionAvail())) {
						Debug::draw_link_maps();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("View matrix", ImGui::GetContentRegionAvail())) {
						Debug::draw_view_matrix();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Entities", ImGui::GetContentRegionAvail())) {
						Debug::draw_entity_list();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Convars", ImGui::GetContentRegionAvail())) {
						Debug::draw_convars();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Local player", ImGui::GetContentRegionAvail())) {
						Debug::draw_local_player();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Game events", ImGui::GetContentRegionAvail())) {
						Debug::draw_event_list();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Panorama", ImGui::GetContentRegionAvail())) {
						Debug::draw_panorama();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Notifications", ImGui::GetContentRegionAvail())) {
						Debug::draw_notifications();
						ImGuiExt::EndBoxedTabItem();
					}

					ImGui::EndTabBar();
				}
				ImGuiExt::EndBoxedTabItem();
			}

			if (ImGuiExt::BeginBoxedTabItem("About", remaining_size)) {
				if (ImGui::BeginTabBar("#About tabs", ImGuiTabBarFlags_Reorderable)) {
					if (ImGuiExt::BeginBoxedTabItem("Configs", ImGui::GetContentRegionAvail())) {
						About::draw_configs();
						ImGuiExt::EndBoxedTabItem();
					}
					if (ImGuiExt::BeginBoxedTabItem("Licenses", ImGui::GetContentRegionAvail())) {
						About::draw_licenses();
						ImGuiExt::EndBoxedTabItem();
					}

					ImGui::EndTabBar();
				}
				ImGuiExt::EndBoxedTabItem();
			}

			ImGui::EndTabBar();
		}

		auto prev_y = ImGui::GetCursorPosY();
		ImGui::Text("FrameworkCS2"
#ifdef DEBUG
					" (debug build)"
#endif

		);

		ImGui::SetCursorPosY(prev_y);
		const char* build_time_string = "Built on " __TIME__ " " __DATE__;
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(build_time_string).x - ImGui::GetStyle().WindowPadding.x);
		ImGui::TextUnformatted(build_time_string);
	}
	ImGui::End();
}
