#include "Tabs.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <string>
#include <utility>

Tabs::Tabs(SettingsHolder* parent, std::string name)
	: Setting(parent, std::move(name))
{
}

void Tabs::render()
{
	if (ImGui::BeginTabBar(get_name().c_str(), ImGuiTabBarFlags_Reorderable)) {
		for (Setting* tab : settings)
			if (ImGui::BeginTabItem(tab->get_name().c_str())) {
				tab->render();
				ImGui::EndTabItem();
			}

		ImGui::EndTabBar();
	}
}

void Tabs::serialize(nlohmann::json& output_json) const
{
	for (Setting* setting : settings)
		setting->serialize(output_json[setting->get_name()]);
}

void Tabs::deserialize(const nlohmann::json& input_json)
{
	for (Setting* setting : settings)
		setting->deserialize(input_json[setting->get_name()]);
}
