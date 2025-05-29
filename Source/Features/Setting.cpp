#include "Setting.hpp"

#include <string>
#include <utility>

void SettingsHolder::render_all_childs()
{
	for (Setting* setting : settings)
		setting->render();
}

Setting::Setting(SettingsHolder* parent, std::string name)
	: name(std::move(name))
{
	parent->settings.push_back(this);
}
