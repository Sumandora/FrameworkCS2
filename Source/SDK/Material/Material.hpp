#pragma once

#include "../VirtualMethod.hpp"

struct Material {
	VIRTUAL_METHOD(0, get_name, const char*, (), (this));
};

