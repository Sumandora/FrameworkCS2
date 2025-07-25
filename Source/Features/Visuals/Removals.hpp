#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/HelpMarker.hpp"

#include "../../Utils/UninitializedObject.hpp"

struct UIPanel;

class Removals : public Feature {
	Checkbox remove_legs{ this, "Remove legs", false };
	Checkbox remove_sleeves{ this, "Remove sleeves", false };
	HelpMarker sleeves_help{ this,
		"While sleeves seem to reappear immediately, removing them requires player models to be reloaded. "
		"This can be accomplished immediately using the \"Force update\" inside the Debug tabs, however that comes with its own drawbacks. "
		"Alternatively, you can just wait for a respawn." };
	Checkbox remove_ads{ this, "Remove ads", false };
	HelpMarker ads_explanation{ this, "Removes ads on the left side of the main menu." };

public:
	Removals();

	[[nodiscard]] bool should_remove_legs() const;
	[[nodiscard]] bool should_remove_sleeves() const;

	void remove_ads_update() const;
};

inline UninitializedObject<Removals> removals;
