#pragma once

#include "PlayerMovementServicesHumanoid.hpp"

#include "../../Schema/FieldOffset.hpp"

struct CSPlayerMovementServices : public PlayerMovementServicesHumanoid {
	CLASS_INFO("libclient.so", "CCSPlayer_MovementServices");

	SCHEMA_VAR(float, duck_amount, "m_flDuckAmount");
	SCHEMA_VAR(float, stamina, "m_flStamina");
};
