#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"
#include <string>

class LoadingTextChanger : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	TextInput text{ this, "Text", "There I am, Gary" };

public:
	LoadingTextChanger();

	[[nodiscard]] bool is_enabled() const;
	[[nodiscard]] const std::string& get_replacement() const;
};
