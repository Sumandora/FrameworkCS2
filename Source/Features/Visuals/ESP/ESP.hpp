#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Button.hpp"
#include "../../Settings/Checkbox.hpp"
#include "../../Settings/Combo.hpp"
#include "../../Settings/MetaSetting.hpp"
#include "../../Settings/Subgroup.hpp"
#include "../../Settings/Tabs.hpp"

#include "imgui.h"

#include "../../../Utils/Logging.hpp"
#include "../../../Utils/UninitializedObject.hpp"

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

	MetaSetting box_meta{ elements, "Box" };
	Checkbox box_enabled{ box_meta, "Enabled", false };
	PlayerRectangle box{ box_meta, "Box" };

	MetaSetting name_meta{ elements, "Name" };
	Checkbox name_enabled{ name_meta, "Enabled", false };
	PlayerSidedText name{ name_meta, "Name" };

	MetaSetting healthbar_meta{ elements, "Health bar" };
	Checkbox healthbar_enabled{ healthbar_meta, "Enabled", false };
	PlayerHealthbar healthbar{ healthbar_meta, "Health bar" };

	MetaSetting skeleton_meta{ elements, "Skeleton" };
	Checkbox skeleton{ skeleton_meta, "Enabled", false };
	PlayerLine skeleton_line{ skeleton_meta, "Line" };

	enum BabbysFirstEnum : std::uint8_t {
		HELLO,
		WORLD
	};

	Combo<BabbysFirstEnum> combo{ this, "Le combo" };

public:
	ESP();

	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<ESP> esp;
