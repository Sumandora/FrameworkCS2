#include "../Tabs.hpp"

#include "../../../Serialization/Serialization.hpp"

#include "../../../Notifications/Notifications.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <chrono>
#include <ctime>
#include <expected>
#include <filesystem>
#include <string>
#include <system_error>

static constexpr const char* CONFIG_CREATION_FAILED_POPUP_ID = "Config creation failed.";
static constexpr const char* CONFIG_LOAD_FAILED_POPUP_ID = "Config load failed.";
static constexpr const char* CONFIG_DELETION_FAILED_POPUP_ID = "Config deletion failed.";
static constexpr const char* CONFIG_OVERWRITE_POPUP_ID = "Overwrite existing config?";

void GUI::Tabs::About::draw_configs()
{
	static std::string config_name;

	static std::expected<void, std::string> config_creation_result;
	static std::error_code config_load_result;
	static std::error_code config_delete_result;

	if (!Serialization::are_configs_available()) {
		ImGui::Text("Configs aren't available. Check standard output.");
		return;
	}

	if (ImGui::Button("Save config")) {
		do {
			if (Serialization::is_reserved_name(config_name)) {
				Notifications::create("Config creation", "Config name is reserved", Notifications::Severity::ERROR);
				break;
			}

			if (Serialization::has_config(config_name)) {
				ImGui::OpenPopup(CONFIG_OVERWRITE_POPUP_ID);
				break;
			}

			config_creation_result = Serialization::create_config(config_name);
			if (!config_creation_result.has_value())
				ImGui::OpenPopup(CONFIG_CREATION_FAILED_POPUP_ID);
		} while (0);
	}

	ImGui::SameLine();

	ImGui::InputText("Config name", &config_name);

	// TODO At some point this should probably be cached and only re-read every now and then, but it's not performance-heavy and I don't feel like making this all lazy
	for (const std::filesystem::path& path : Serialization::get_available_configs()) {
		const std::string name = path.stem();
		if (ImGui::BeginChild(("##" + name).c_str(), {},
				ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize)) {
			do {
				std::error_code last_modified_error;
				const std::filesystem::file_time_type last_modified = std::filesystem::last_write_time(path, last_modified_error);
				if (last_modified_error)
					break; // TODO Better error handling

				// TODO Clang doesn't like this... Can this be done without clock_cast
				const std::chrono::system_clock::time_point system_time = std::chrono::template clock_cast<std::chrono::system_clock>(last_modified);
				const std::time_t native_time = std::chrono::system_clock::to_time_t(system_time);

				char time[128]{};
				// Match with GCC build time/date strings for consistency
				if (!std::strftime(time, sizeof(time), "%T %b %d %Y", std::localtime(&native_time)))
					break;

				const ImVec2 begin_cursor = ImGui::GetCursorPos();

				static constexpr auto RIGHT_ALIGNED_BUTTON = [](const char* text, float* button_height) {
					const ImVec2 button_size = ImGui::CalcTextSize(text) + ImGui::GetStyle().FramePadding * 2.0F;
					if (button_height)
						*button_height = button_size.y;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - button_size.x);
					return ImGui::Button(text);
				};

				float first_button_height = 0.0F;

				if (RIGHT_ALIGNED_BUTTON("Load config", &first_button_height)) {
					const bool success = Serialization::load_config(name, config_load_result);
					if (!success) {
						ImGui::OpenPopup(CONFIG_LOAD_FAILED_POPUP_ID);
					}
				}

				const float second_button_cursor_height = ImGui::GetCursorPosY();
				float second_button_height = 0.0F;

				if (RIGHT_ALIGNED_BUTTON("Delete config", &second_button_height)) {
					const bool success = std::filesystem::remove(path, config_delete_result);
					if (!success) {
						ImGui::OpenPopup(CONFIG_DELETION_FAILED_POPUP_ID);
					}
				}

				ImGui::SetCursorPos(begin_cursor);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + first_button_height / 2.0F - ImGui::GetTextLineHeight() / 2.0F);
				ImGui::TextUnformatted(name.c_str());

				// I think this makes the name stand out a bit more
				ImGui::SetCursorPosY(second_button_cursor_height + second_button_height / 2.0F - ImGui::GetTextLineHeight() / 2.0F);
				ImGui::Text("Last modified: %s", time);
			} while (false);
		}
		ImGui::EndChild();
	}

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));
	if (ImGui::BeginPopupModal(CONFIG_OVERWRITE_POPUP_ID, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
		ImGui::Text("Config '%s' already exists. Do you want to overwrite it?", config_name.c_str());
		ImGui::Separator();

		const float width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0F;

		if (ImGui::Button("Yes", { width, 0.0F })) {
			ImGui::CloseCurrentPopup();
			config_creation_result = Serialization::create_config(config_name);
			if (!config_creation_result.has_value())
				ImGui::OpenPopup(CONFIG_CREATION_FAILED_POPUP_ID);
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("No", { width, 0.0F })) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));
	if (ImGui::BeginPopupModal(CONFIG_CREATION_FAILED_POPUP_ID, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
		ImGui::TextUnformatted(config_creation_result.error().c_str());
		ImGui::EndPopup();
	}

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));
	if (ImGui::BeginPopupModal(CONFIG_LOAD_FAILED_POPUP_ID, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
		ImGui::TextUnformatted(config_load_result.message().c_str());
		ImGui::EndPopup();
	}

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5F, 0.5F));
	if (ImGui::BeginPopupModal(CONFIG_DELETION_FAILED_POPUP_ID, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings)) {
		ImGui::TextUnformatted(config_delete_result.message().c_str());
		ImGui::EndPopup();
	}
}
