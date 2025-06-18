#pragma once

#include "../VirtualMethod.hpp"

struct ViewRender {
	VIRTUAL_METHOD(4, on_render_start, void, (), (this));
};
