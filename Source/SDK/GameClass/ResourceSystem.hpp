#pragma once

#include "../VirtualMethod.hpp"

class ResourceSystem {
public:
	VIRTUAL_METHOD(2, query_interface, void*, (const char* name), (this, name));
};
