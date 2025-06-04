#include "TextInput.hpp"

#include "../Setting.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <string>
#include <utility>

TextInput::TextInput(SettingsHolder* parent, std::string name, std::string default_value)
	: Setting(parent, std::move(name))
	, value(std::move(default_value))
{
}

void TextInput::render()
{
	ImGui::InputText(get_name().c_str(), &value);
}

void TextInput::serialize(nlohmann::json& output_json) const
{
	output_json = value;
}

void TextInput::deserialize(const nlohmann::json& input_json)
{
	value = input_json;
}
