#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"

class ForceCrosshair : public Feature {
	Checkbox enabled = make_checkbox("Enabled");

public:
	ForceCrosshair();

	[[nodiscard]] bool should_force() const;
};

