#pragma once

#include "../../Schema/FieldOffset.hpp"

#include "PlayerMovementServices.hpp"

struct PlayerMovementServicesHumanoid : public PlayerMovementServices {
	CLASS_INFO("libclient.so", "CPlayer_MovementServices_Humanoid");

	SCHEMA_VAR(bool, ducked, "m_bDucked");
	SCHEMA_VAR(bool, ducking, "m_bDucking"); // in process of ducking; false when fully ducked.
	SCHEMA_VAR(float, surface_friction, "m_flSurfaceFriction");
};
