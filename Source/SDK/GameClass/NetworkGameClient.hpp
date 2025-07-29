#pragma once

#include "../VirtualMethod.hpp"

#include "../Padding.hpp"

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
	PADDING(0x254);

public:
	// NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
	int delta_tick;

	static void resolve_signatures();

	void client_side_predict(PredictionStage stage);
};
