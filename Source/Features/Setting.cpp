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
