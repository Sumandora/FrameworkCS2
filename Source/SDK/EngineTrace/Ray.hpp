#pragma once

#include "../Padding.hpp"

struct Ray {
	PADDING(0x29);
};

static_assert(sizeof(Ray) == 0x29);
