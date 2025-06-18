#pragma once

#include "../VirtualMethod.hpp"

#include "ClientFrameStage.hpp"

class Source2Client {
public:
	VIRTUAL_METHOD(36, frame_stage_notify, void, (ClientFrameStage stage), (this, stage));
};
