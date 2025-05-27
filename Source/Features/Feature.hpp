#pragma once

#include <string>

#include "Setting.hpp"

class Feature : public SettingsHolder {
	std::string category;
	std::string name;

public:
	explicit Feature(std::string category, std::string name);

	Feature(const Feature&) = delete;
	Feature& operator=(const Feature&) = delete;

	void render();

	[[nodiscard]] const std::string& get_category() const { return category; }
	[[nodiscard]] const std::string& get_name() const { return name; }
};
