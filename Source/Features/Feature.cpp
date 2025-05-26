#include "Feature.hpp"

#include "Setting.hpp"

#include "Features.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

Feature::Feature(std::string category, std::string name)
	: category(std::move(category))
	, name(std::move(name))
{
	auto it = std::ranges::find(Features::features, this->category, [](const auto& pair) {
		return pair.first;
	});

	if (it == Features::features.end())
		it = Features::features.insert(it, { this->category, { this } });
	else
		it->second.emplace_back(this);
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
