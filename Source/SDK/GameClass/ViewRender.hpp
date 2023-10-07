#pragma once

#include "../VirtualMethod.hpp"

struct ViewRender {
	VIRTUAL_METHOD(4, onRenderStart, void, (), (this));
};