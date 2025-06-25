#pragma once

#include "../Padding.hpp"

struct GlobalVars {
	PADDING(0x188);
};

static_assert(sizeof(GlobalVars) == 0x188); // Probably incorrect...
