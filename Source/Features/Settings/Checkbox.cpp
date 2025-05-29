#include "../Setting.hpp"

#include "imgui.h"

#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#include <string>
#include <utility>

Checkbox::Checkbox(SettingsHolder* parent, std::string name, bool value)
	: Setting(parent, std::move(name))
	, value(value)
{
}

void Checkbox::render()
{
	ImGui::Checkbox(get_name().c_str(), &value);
}

void Checkbox::serialize(nlohmann::json& output_json) const
{
	output_json = value;
}

void Checkbox::deserialize(const nlohmann::json& input_json)
{
	value = input_json;
}
