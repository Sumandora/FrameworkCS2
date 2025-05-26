#pragma once

#include <string>
#include <vector>

#include "Setting.hpp"

class Feature {
	std::vector<Setting*> settings;
	friend Setting;

	std::string category;
	std::string name;

public:
	explicit Feature(std::string category, std::string name);

	Feature(const Feature&) = delete;
	Feature& operator=(const Feature&) = delete;

protected:
	Checkbox make_checkbox(std::string name);

public:
	void render();

	[[nodiscard]] const std::string& get_category() const { return category; }
	[[nodiscard]] const std::string& get_name() const { return name; }
};
