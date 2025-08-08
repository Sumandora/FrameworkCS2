#pragma once

#include "../VirtualMethod.hpp"

class RenderingPipelineCsgoPostHud {
public:
	VIRTUAL_METHOD(0, add_layers, void,
		(void* unk1, void* unk2, void* unk3, void* unk4, void* unk5),
		(this, unk1, unk2, unk3, unk4, unk5));
};
