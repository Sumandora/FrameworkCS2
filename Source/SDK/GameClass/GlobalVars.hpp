#pragma once

#include "../Padding.hpp"

struct GlobalVars {
	PADDING(0x188);

	OFFSET(float, current_time, 0x30);
};

static_assert(sizeof(GlobalVars) == 0x188); // Probably incorrect...
