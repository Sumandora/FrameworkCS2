#pragma once

#include "../VirtualMethod.hpp"

#include "../Enums/ClientFrameStage.hpp"

struct EconItemSystem;

class Source2Client {
public:
	VIRTUAL_METHOD(36, frame_stage_notify, void, (ClientFrameStage stage), (this, stage));
	VIRTUAL_METHOD(122, get_econ_item_system, EconItemSystem*, (), (this));
};
