#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"

#include "imgui.h"

class ESP : public Feature {
	Checkbox enabled = make_checkbox("Enabled");

public:
	ESP();

	void draw(ImDrawList* draw_list);
};

