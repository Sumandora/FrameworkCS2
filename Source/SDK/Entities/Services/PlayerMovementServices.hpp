#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "../../VirtualMethod.hpp"
#include "glm/ext/vector_float3.hpp"

struct UserCmd;

struct PlayerMovementServices {
	CLASS_INFO("libclient.so", "CPlayer_MovementServices");

	SCHEMA_VAR(float, max_speed, "m_flMaxspeed");

	SCHEMA_VAR(glm::vec3, last_movement_impulses, "m_vecLastMovementImpulses");

	VIRTUAL_METHOD(37, set_prediction_command, void, (UserCmd * cmd), (this, cmd));
	VIRTUAL_METHOD(38, reset_prediction_command, void, (), (this));
};
