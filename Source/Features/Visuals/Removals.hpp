#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/HelpMarker.hpp"

#include "../../Utils/UninitializedObject.hpp"

struct UIPanel;

class Removals : public Feature {
	Checkbox remove_legs{ this, "Remove legs", false };
	Checkbox remove_ads{ this, "Remove ads", false };
	HelpMarker ads_explanation{ this, "Removes ads on the left side of the main menu." };

public:
	Removals();

	[[nodiscard]] bool should_remove_legs() const;

	void remove_ads_update() const;
};

inline UninitializedObject<Removals> removals;
