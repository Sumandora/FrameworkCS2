#include "Color.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <string>
#include <utility>

Color::Color(SettingsHolder* parent, std::string name, ImColor value)
	: Setting(parent, std::move(name))
	, value(value)
{
}

void Color::render()
{

	const bool clicked = ImGui::ColorButton((get_name() + "##Button").c_str(), value, ImGuiColorEditFlags_None, ImVec2(0, 0));
	ImGui::SameLine();
	ImGui::Text("%s", get_name().c_str());

	auto id = get_name() + "##Popup";
	if (clicked)
		ImGui::OpenPopup(id.c_str());

	if (ImGui::BeginPopup(id.c_str())) {
		float float_array[] = { value.Value.x, value.Value.y, value.Value.z, value.Value.w };
		if (ImGui::ColorPicker4("##Picker", float_array, 0)) {
			value.Value = ImVec4(float_array[0], float_array[1], float_array[2], float_array[3]);
		}
		ImGui::EndPopup();
	}
}

void Color::serialize(nlohmann::json& output_json) const
{
	output_json = { value.Value.x, value.Value.y, value.Value.z, value.Value.w };
}

void Color::deserialize(const nlohmann::json& input_json)
{
	value.Value.x = input_json[0];
	value.Value.y = input_json[1];
	value.Value.z = input_json[2];
	value.Value.w = input_json[3];
}
