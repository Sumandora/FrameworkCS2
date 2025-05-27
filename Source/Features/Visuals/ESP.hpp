#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"

#include "imgui.h"

#include "../../Utils/Logging.hpp"

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled" };
	Subgroup lotto{ this, "Lotto" };
	Checkbox another_check{ lotto, "Another check" };
	Button btn{lotto, "Click me", []() {
		Logging::info("heya");
	}};

public:
	ESP();

	void draw(ImDrawList* draw_list);
};
