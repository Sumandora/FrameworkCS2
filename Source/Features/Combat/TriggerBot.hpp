#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"
#include "../Settings/Subgroup.hpp"

#include "../../Utils/UninitializedObject.hpp"

class UserCmd;

class TriggerBot : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	FloatSlider min_damage{ this, "Min damage", 0.0F, 100.0F, 50.0F };
	Subgroup damage_subgroup{ this, "Damage subgroup", true };
	Checkbox disable_penetration{ damage_subgroup, "Disable penetration", false };

public:
	TriggerBot();

	void create_move(UserCmd* cmd);
};

inline UninitializedObject<TriggerBot> trigger_bot;
