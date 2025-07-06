#pragma once

#include "../VirtualMethod.hpp"

// NOLINTNEXTLINE(performance-enum-size)
enum class PredictionStage : int {
	// Determined through enum -> string conversion inside the ClientSidePredict function
	CLIENT_COMMAND_TICK = 0,
	DEMO_PREENTITY,
	DEMO_SIMULATION,
	POSTNETUPDATE,
	SERVER_STARVED_AND_ADDED_USERCMD,
	CLIENT_FRAME_SIMULATE,
};

class NetworkGameClient {
public:
	void client_side_predict(PredictionStage stage);

	VIRTUAL_METHOD(28, force_full_update, void, (const char* reason), (this, reason));
};
