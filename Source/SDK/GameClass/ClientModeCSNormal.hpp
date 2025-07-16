#pragma once

#include "../VirtualMethod.hpp"

struct ViewSetup;

class ClientModeCSNormal {
public:
	VIRTUAL_METHOD(16, override_view, void, (ViewSetup * setup), (this, setup));
};
