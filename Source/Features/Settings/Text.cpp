#include "Text.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <string>
#include <utility>

Text::Text(SettingsHolder* parent, std::string content)
	: Setting(parent, "Help Marker")
	, content(std::move(content))
{
}

void Text::render()
{
	ImGui::TextUnformatted(content.c_str());
}

void Text::serialize(nlohmann::json& /*output_json*/) const
{
	// TODO don't save a null value.
}

void Text::deserialize(const nlohmann::json& /*input_json*/)
{
}
