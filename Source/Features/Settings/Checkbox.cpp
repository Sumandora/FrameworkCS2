#include "Checkbox.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <string>
#include <utility>

RawCheckbox::RawCheckbox(SettingsHolder* parent, std::string name, bool value)
	: Setting(parent, std::move(name))
	, value(value)
{
}

void RawCheckbox::render()
{
	ImGui::Checkbox(get_name().c_str(), &value);
}

void RawCheckbox::serialize(nlohmann::json& output_json) const
{
	output_json = value;
}

void RawCheckbox::deserialize(const nlohmann::json& input_json)
{
	value = input_json;
}
