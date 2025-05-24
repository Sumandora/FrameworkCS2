#include "Feature.hpp"

#include "Setting.hpp"

#include <string>
#include <utility>

Feature::Feature(std::string name)
	: name(std::move(name))
{
}

Checkbox Feature::make_checkbox(std::string name)
{
	return Checkbox{ this, std::move(name) };
}

void Feature::render()
{
	for (Setting* setting : settings)
		setting->render();
}
