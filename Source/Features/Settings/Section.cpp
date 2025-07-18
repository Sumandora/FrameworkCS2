#include "Section.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <string>
#include <utility>

Section::Section(SettingsHolder* parent, std::string name)
	: Setting(parent, std::move(name))
{
}

void Section::render()
{
	const std::string& name = get_name();
	ImGui::SeparatorText(name.c_str());

	ImGui::PushID(name.c_str());
	render_all_childs();
	ImGui::PopID();
}

void Section::serialize(nlohmann::json& output_json) const
{
	SettingsHolder::serialize(output_json);
}

void Section::deserialize(const nlohmann::json& input_json)
{
	SettingsHolder::deserialize(input_json);
}
