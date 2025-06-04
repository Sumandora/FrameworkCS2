#include "MetaSetting.hpp"

#include "../Setting.hpp"

#include <string>
#include <utility>

MetaSetting::MetaSetting(SettingsHolder* parent, std::string name)
	: Setting(parent, std::move(name))
{
}

void MetaSetting::render()
{
	render_all_childs();
}

void MetaSetting::serialize(nlohmann::json& output_json) const
{
	for (Setting* setting : settings)
		setting->serialize(output_json[setting->get_name()]);
}

void MetaSetting::deserialize(const nlohmann::json& input_json)
{
	for (Setting* setting : settings)
		setting->deserialize(input_json[setting->get_name()]);
}
