#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

class ForceCrosshair : public Feature {
	Checkbox enabled{ this, "Enabled", false };

public:
	ForceCrosshair();

	[[nodiscard]] bool should_force() const;
};

inline UninitializedObject<ForceCrosshair> force_crosshair;
