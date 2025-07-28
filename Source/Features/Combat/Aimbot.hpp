#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"
#include "../Settings/HelpMarker.hpp"
#include "../Settings/Subgroup.hpp"

#include "../../Utils/UninitializedObject.hpp"

class UserCmd;

class Aimbot : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	FloatSlider maximum_fov{ this, "Maximum FOV", 0.0F, 255.0F, 255.0F }; // TODO maybe this slider should be logarithmic
	FloatSlider min_damage{ this, "Min damage", 0.0F, 100.0F, 50.0F };
	Subgroup damage_subgroup{ this, "Damage subgroup", true };
	Checkbox disable_penetration{ damage_subgroup, "Disable penetration", false };
	Checkbox aim_between_shots{ this, "Aim between shots", false };
	HelpMarker aim_between_shots_performance_warning{ this,
		"This implies that to aim at someone, that certain someone needs to be calculated. "
		"This may lead to a performance hit." };
	Checkbox silent_aim{ this, "Silent aim", true };
	Checkbox smoke_check{ this, "Smoke check", false };
	FloatSlider max_smoke_density{ this, "Max smoke density", 0.0F, 8.0F, 0.2F }; // TODO maybe this slider should be logarithmic; This would also be a candidate to be hidden under a "Advanced settings" option
	HelpMarker smoke_density_explanation{ this,
		"Smoke density refers to how much of the ray traveled through a smoke. 0.0 means no smoke at all. "
		"Every value above zero means that the ray at least partially traveled through a smoke. "
		"The higher this value is, the less sensitive the Aimbot will be to smokes." };

public:
	Aimbot();

	void create_move(UserCmd* cmd);

	bool wants_silent_aim() const;
};

inline UninitializedObject<Aimbot> aimbot;
