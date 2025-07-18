#include "Subgroup.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <string>
#include <utility>

Subgroup::Subgroup(SettingsHolder* parent, std::string name, bool anonymous)
	: Setting(parent, std::move(name))
	, anonymous(anonymous)
{
}

void Subgroup::render()
{
	if (!anonymous)
		ImGui::Text("%s", get_name().c_str());

	ImGui::SameLine();

	const std::string popup_id = get_name() + "##Popup";
	if (ImGui::Button(("...##" + get_name()).c_str()))
		ImGui::OpenPopup(popup_id.c_str());

	if (ImGui::BeginPopup(popup_id.c_str())) {
		render_all_childs();

		ImGui::EndPopup();
	}
}

void Subgroup::serialize(nlohmann::json& output_json) const
{
	SettingsHolder::serialize(output_json);
}

void Subgroup::deserialize(const nlohmann::json& input_json)
{
	SettingsHolder::deserialize(input_json);
}
