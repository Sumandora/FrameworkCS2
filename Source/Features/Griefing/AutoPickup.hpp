#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Text.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include <string>

class AutoPickup : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Text auto_pickup_explanation{ this,
		"Automatically picks up better weapons using the buy menu. "
		"Better is defined as in a higher price." };

public:
	AutoPickup();

	void update();
};

inline UninitializedObject<AutoPickup> auto_pickup;
