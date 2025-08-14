#include "Color.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <array>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>

RawColor::RawColor(SettingsHolder* parent, std::string name, ImColor value)
	: Setting(parent, std::move(name))
	, value(value)
{
}

void RawColor::render()
{
	const bool clicked = ImGui::ColorButton((get_name() + "##Button").c_str(), value, ImGuiColorEditFlags_None, ImVec2(0, 0));
	ImGui::SameLine();
	ImGui::Text("%s", get_name().c_str());

	auto id = get_name() + "##Popup";
	if (clicked)
		ImGui::OpenPopup(id.c_str());

	if (ImGui::BeginPopup(id.c_str())) {
		std::array<float, 4> float_array;
		{
			const std::shared_lock lock{ mutex };
			float_array = { value.Value.x, value.Value.y, value.Value.z, value.Value.w };
		}
		if (ImGui::ColorPicker4("##Picker", float_array.data())) {
			const std::unique_lock lock{ mutex };
			value.Value = ImVec4(float_array[0], float_array[1], float_array[2], float_array[3]);
		}
		ImGui::EndPopup();
	}
}

void RawColor::serialize(nlohmann::json& output_json) const
{
	const std::unique_lock lock{ mutex };
	output_json = { value.Value.x, value.Value.y, value.Value.z, value.Value.w };
}

void RawColor::deserialize(const nlohmann::json& input_json)
{
	const std::unique_lock lock{ mutex };
	value = {
		input_json[0].get<float>(),
		input_json[1].get<float>(),
		input_json[2].get<float>(),
		input_json[3].get<float>(),
	};
}
