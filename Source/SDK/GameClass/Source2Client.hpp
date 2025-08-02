#pragma once

#include "../VirtualMethod.hpp"

#include "../Enums/ClientFrameStage.hpp"

struct EconItemSystem;

class Source2Client {
public:
	// Ghidra miss-detects a 64-bit int, but judging by the assembly, this is just an int
	VIRTUAL_METHOD(18, get_command_index, int, (int slot), (this, slot));
	VIRTUAL_METHOD(36, frame_stage_notify, void, (ClientFrameStage stage), (this, stage));
	VIRTUAL_METHOD(122, get_econ_item_system, EconItemSystem*, (), (this));
};
