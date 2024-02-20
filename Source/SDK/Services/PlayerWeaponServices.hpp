#pragma once

#include "../Entities/Weapon/BasePlayerWeapon.hpp"
#include "../GameClass/Handle.hpp"
#include "../GameClass/NetworkUtlVectorBase.hpp"
#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"

struct PlayerWeaponServices {
	CLASS_INFO("libclient.so", "CPlayer_WeaponServices");

	// For inheritance
protected:
	SCHEMA_VAR(Handle<BasePlayerWeapon>, activeWeapon, "m_hActiveWeapon")
	SCHEMA_VAR(NetworkUtlVectorBase<Handle<BasePlayerWeapon>>, weapons, "m_hMyWeapons")
};