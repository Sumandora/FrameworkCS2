#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include <chrono>

class AutoAccept : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	FloatSlider delay{ this, "Delay", 0.0F, 10.0F, 1.0F };
	// TODO system notifications

	std::chrono::system_clock::time_point match_found_time;
	bool awaits_accept = false;

public:
	AutoAccept();

	void on_match_found();
	void update();
};

inline UninitializedObject<AutoAccept> auto_accept;
