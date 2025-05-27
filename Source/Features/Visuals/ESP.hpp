#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"

#include "imgui.h"

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled" };
	Subgroup lotto{ this, "Lotto" };
	Checkbox another_check{ lotto, "Another check" };

public:
	ESP();

	void draw(ImDrawList* draw_list);
};
