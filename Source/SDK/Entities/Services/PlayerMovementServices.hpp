#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "../../VirtualMethod.hpp"

struct UserCmd;

struct PlayerMovementServices {
	CLASS_INFO("libclient.so", "CPlayer_MovementServices");

	VIRTUAL_METHOD(37, set_prediction_command, void, (UserCmd* cmd), (this, cmd));
	VIRTUAL_METHOD(38, reset_prediction_command, void, (), (this));
};
