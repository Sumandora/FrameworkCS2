#pragma once

#include "PlayerWeaponServices.hpp"

#include "../../VirtualMethod.hpp"

#include "../../Schema/FieldOffset.hpp"

#include "../../GameClass/GameTime.hpp"

#include "glm/ext/vector_float3.hpp"

struct CSPlayerWeaponServices : public PlayerWeaponServices {
	CLASS_INFO("libclient.so", "CCSPlayer_WeaponServices");

	SCHEMA_VAR(GameTime, next_attack, "m_flNextAttack");

	VIRTUAL_METHOD(27, get_shoot_position, glm::vec3, (), (this));

	// Updates and then calls the get_shoot_position vfunc
	// TODO No use for this now...
	glm::vec3 get_interpolated_shoot_position(int tick, float fraction);
};
