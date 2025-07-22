#pragma once

#include "PlayerWeaponServices.hpp"

#include "../../Schema/FieldOffset.hpp"

#include "../../GameClass/GameTime.hpp"

struct CSPlayerWeaponServices : public PlayerWeaponServices {
	CLASS_INFO("libclient.so", "CCSPlayer_WeaponServices");

	SCHEMA_VAR(GameTime, next_attack, "m_flNextAttack");
};
