#pragma once

#include "../VirtualMethod.hpp"

struct GameEvent {
	VIRTUAL_METHOD(2, GetName, char*, (), (this));
};