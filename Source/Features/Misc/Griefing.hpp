#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/HelpMarker.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include <string>

class Griefing : public Feature {
	Checkbox auto_pickup{ this, "Auto pickup", false };
	HelpMarker auto_pickup_explanation{ this,
		"Automatically picks up better weapons using the buy menu. "
		"Better is defined as in a higher price." };

public:
	Griefing();

	void update();
};

inline UninitializedObject<Griefing> griefing;
