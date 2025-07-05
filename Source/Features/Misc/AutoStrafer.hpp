#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Combo.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include <cstdint>

class UserCmd;

enum class AutoStraferMode : std::uint8_t {
	// NOLINTBEGIN(readability-identifier-naming)
	Directional,
	Forward,
	// NOLINTEND(readability-identifier-naming)
};

class AutoStrafer : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Combo<AutoStraferMode> mode{ this, "Mode", AutoStraferMode::Directional };
	Checkbox allow_hard_turns{ this, "Allow hard turns", true };
	FloatSlider hard_turn_threshold{ this, "Hard turn threshold", 0.0F, 180.0F, 135.0F };
	Checkbox subtick{ this, "Subtick (WIP)", true };
	Checkbox only_when_idle{ this, "Only when idle", false };

public:
	AutoStrafer();

	void create_move(UserCmd* cmd);
};

inline UninitializedObject<AutoStrafer> auto_strafer;
