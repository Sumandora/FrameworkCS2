#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

class Removals : public Feature {
	Checkbox remove_legs{ this, "Remove legs", false };
	Checkbox remove_sleeves{ this, "Remove sleeves", false };

public:
	Removals();

	[[nodiscard]] bool should_remove_legs() const;
	[[nodiscard]] bool should_remove_sleeves() const;
};

inline UninitializedObject<Removals> removals;
