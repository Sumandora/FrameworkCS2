#pragma once

struct ImDrawList;
struct UserCmd;

namespace MovementQuantization {
	void init();

	bool is_movement_quantized();
	void circumvent_quantization(UserCmd* cmd);
}
