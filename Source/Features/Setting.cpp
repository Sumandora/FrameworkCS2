#include "Setting.hpp"

#include <string>
#include <utility>

Setting::Setting(SettingsHolder* parent, std::string name)
	: name(std::move(name))
{
	parent->settings.push_back(this);
}
