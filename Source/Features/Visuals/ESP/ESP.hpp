#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Button.hpp"
#include "../../Settings/Checkbox.hpp"
#include "../../Settings/Combo.hpp"
#include "../../Settings/Subgroup.hpp"
#include "../../Settings/Tabs.hpp"

#include "imgui.h"

#include "../../../SDK/Entities/BaseEntity.hpp"
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
	PlayerBar healthbar{ elements, "Health bar",
		[](const BaseEntity* e) { return static_cast<float>(e->health()) / static_cast<float>(e->max_health()); },
		[](const BaseEntity* e) { return std::to_string(e->health()); } };

	enum BabbysFirstEnum : std::uint8_t {
		HELLO,
		WORLD
	};

	Combo<BabbysFirstEnum> combo{ this, "Le combo" };

public:
	ESP();

	void draw(ImDrawList* draw_list);
};
