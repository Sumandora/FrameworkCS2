#include "Setting.hpp"

#include <functional>
#include <string>
#include <utility>

void SettingsHolder::render_all_childs()
{
	for (Setting* setting : settings)
		if (setting->is_visible())
			setting->render();
}

void SettingsHolder::serialize(nlohmann::json& output_json) const
{
	for (Setting* setting : settings)
		setting->serialize(output_json[setting->get_name()]);
}

void SettingsHolder::deserialize(const nlohmann::json& input_json)
{
	for (Setting* setting : settings)
		if (input_json.contains(setting->get_name()))
			setting->deserialize(input_json[setting->get_name()]);
}

Setting::Setting(SettingsHolder* parent, std::string name)
	: name(std::move(name))
	, visible([] { return true; })
{
	parent->settings.push_back(this);
}

bool Setting::is_visible()
{
	return visible();
}

void Setting::add_visible_condition(std::function<bool()> visible) &
{
	this->visible = std::move(visible);
}
