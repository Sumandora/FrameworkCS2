#include "Color.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <atomic>
#include <string>
#include <utility>

RawColor::RawColor(SettingsHolder* parent, std::string name, ImColor value)
	: Setting(parent, std::move(name))
	, value(value)
{
}

void RawColor::render()
{
	ImColor value = this->value.load(std::memory_order::acquire);
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
			this->value.store(value);
		}
		ImGui::EndPopup();
	}
}

void RawColor::serialize(nlohmann::json& output_json) const
{
	ImColor value = this->value.load();
	output_json = { value.Value.x, value.Value.y, value.Value.z, value.Value.w };
}

void RawColor::deserialize(const nlohmann::json& input_json)
{
	const ImColor color{
		input_json[0].get<float>(),
		input_json[1].get<float>(),
		input_json[2].get<float>(),
		input_json[3].get<float>(),
	};
	value.store(color);
}
