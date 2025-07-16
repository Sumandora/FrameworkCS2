#pragma once

struct UserCmd;

namespace Prediction {
	// @returns if successful
	bool begin(UserCmd* cmd);
	// Don't call end, if begin returned false
	void end();

	bool in_prediction();
}
