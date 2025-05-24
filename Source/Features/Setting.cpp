#include "Setting.hpp"

#include <string>
#include <utility>

#include "Feature.hpp"

Setting::Setting(Feature* parent, std::string name)
	: name(std::move(name))
{
	parent->settings.push_back(this);
}
