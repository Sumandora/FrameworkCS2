#pragma once

#include "../VirtualMethod.hpp"

struct UserCmd;

class CSGOInput {
public:
	VIRTUAL_METHOD(22, create_move, void*, (int unk, UserCmd* cmd), (this, unk, cmd));
};
