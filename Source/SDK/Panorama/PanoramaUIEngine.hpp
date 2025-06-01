#pragma once

#include "../VirtualMethod.hpp"

struct UIEngine;

struct PanoramaUIEngine {
	VIRTUAL_METHOD(13, access_ui_engine, UIEngine*, (), (this));
};
