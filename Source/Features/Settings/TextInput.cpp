#include "TextInput.hpp"

#include "../Setting.hpp"

#include "imgui.h" // IWYU pragma: keep
#include "misc/cpp/imgui_stdlib.h"

#include <string>
#include <utility>

RawTextInput::RawTextInput(SettingsHolder* parent, std::string name, std::string default_value)
	: Setting(parent, std::move(name))
	, value(std::move(default_value))
{
}

void RawTextInput::render()
{
	ImGui::InputText(get_name().c_str(), &value);
}

void RawTextInput::serialize(nlohmann::json& output_json) const
{
	output_json = value;
}

void RawTextInput::deserialize(const nlohmann::json& input_json)
{
	value = input_json;
}
