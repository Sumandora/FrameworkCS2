#pragma once

#include "../../Feature.hpp"
#include "../../Setting.hpp"

#include "imgui.h"

#include "../../../Utils/Logging.hpp"

#include <cstdint>
#include <string>

#include "Elements.hpp"

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Subgroup lotto{ this, "Lotto" };
	Checkbox another_check{ lotto, "Another check", true };
	Button btn{ lotto, "Click me", []() {
				   Logging::info("heya");
			   } };

	Tabs elements{ this, "Elements" };
	PlayerRectangle box{ elements, "Box" };
	PlayerSidedText name{ elements, "Name" };

	enum BabbysFirstEnum : std::uint8_t {
		HELLO,
		WORLD
	};

	Combo<BabbysFirstEnum> combo{ this, "Le combo" };

public:
	ESP();

	void draw(ImDrawList* draw_list);
};
