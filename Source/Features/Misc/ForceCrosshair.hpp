#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"

class ForceCrosshair : public Feature {
	Checkbox enabled{ this, "Enabled" };

public:
	ForceCrosshair();

	[[nodiscard]] bool should_force() const;
};
