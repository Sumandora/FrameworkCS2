#pragma once

#include "../../Schema/FieldOffset.hpp"

#include "PlayerMovementServices.hpp"

struct PlayerMovementServicesHumanoid : public PlayerMovementServices {
	CLASS_INFO("libclient.so", "CPlayer_MovementServices_Humanoid");

	SCHEMA_VAR(float, surface_friction, "m_flSurfaceFriction");
};
