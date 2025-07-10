#pragma once

#include "../VirtualMethod.hpp"

class Localize {
public:
	VIRTUAL_METHOD(18, translate, const char*, (const char* localization_key), (this, localization_key));
};
