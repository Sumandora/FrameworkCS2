#pragma once

#include "../VirtualMethod.hpp"

struct UserCmd;

class CSGOInput {
public:
	VIRTUAL_METHOD(6, create_move, void*, (int unk1, char unk2), (this, unk1, unk2));
};
