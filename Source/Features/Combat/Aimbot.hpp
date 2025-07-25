#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

class UserCmd;

class Aimbot : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	FloatSlider maximum_fov{ this, "Maximum FOV", 0.0F, 255.0F, 255.0F };
	FloatSlider min_damage{ this, "Min damage", 0.0F, 100.0F, 50.0F };

public:
	Aimbot();

	void create_move(UserCmd* cmd);

	bool wants_silent_aim() const;
};

inline UninitializedObject<Aimbot> aimbot;
