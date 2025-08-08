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
	Checkbox remove_scope{ this, "Remove scope", false };
	Checkbox remove_flash_overlay{ this, "Remove flash overlay", false };

public:
	Removals();

	[[nodiscard]] bool should_remove_legs() const;
	[[nodiscard]] bool should_remove_scope() const;
	[[nodiscard]] bool should_remove_flash_overlay() const;

	void remove_ads_update() const;
};

inline UninitializedObject<Removals> removals;
