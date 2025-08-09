#pragma once

#include "../../CUtl/Vector.hpp"
#include "../../EntityHandle.hpp"
#include "../../Schema/FieldOffset.hpp"

struct BasePlayerWeapon;

struct PlayerWeaponServices {
	CLASS_INFO("libclient.so", "CPlayer_WeaponServices");

	SCHEMA_VAR(UtlVector<EntityHandle<BasePlayerWeapon>>, my_weapons, "m_hMyWeapons");
	SCHEMA_VAR(EntityHandle<BasePlayerWeapon>, active_weapon, "m_hActiveWeapon");
};
