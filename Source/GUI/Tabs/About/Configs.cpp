#include "../Tabs.hpp"

#include "../../../Serialization/Serialization.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <expected>
#include <string>
#include <system_error>

void GUI::Tabs::About::draw_configs()
{
	if (!Serialization::are_configs_available()) {
		ImGui::Text("Configs aren't available. Check standard output.");
		return;
	}

	static std::string config_name;
	ImGui::InputText("Config name", &config_name);

	static std::expected<void, std::string> config_creation_result;
	if (ImGui::Button("Save config")) {
		config_creation_result = Serialization::create_config(config_name, true /* TODO */);
		if (!config_creation_result.has_value()) {
			ImGui::OpenPopup("Config creation failed.");
		}
	}

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

	if (ImGui::BeginPopupModal("Config creation failed.", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextUnformatted(config_creation_result.error().c_str());
		ImGui::EndPopup();
	}

	static std::error_code config_load_result;
	if (ImGui::Button("Load config")) {
		const bool success = Serialization::load_config(config_name, config_load_result);
		if (!success) {
			ImGui::OpenPopup("Config load failed.");
		}
	}
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));

	if (ImGui::BeginPopupModal("Config load  failed.", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextUnformatted(config_load_result.message().c_str());
		ImGui::EndPopup();
	}
}
