#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "../../Schema/SchemaClassInfo.hpp"
#include "../../Services/PlayerWeaponServices.hpp"
#include "../BaseEntity.hpp"

struct BasePlayerPawn : BaseEntity {
	CLASS_INFO("libclient.so", "C_BasePlayerPawn");

	// This is protected because we only care about the case of CSPlayerPawnBase,
	// where weaponServices is actually CSPlayerWeaponServices
protected:
	SCHEMA_VAR(PlayerWeaponServices*, weaponServices, "m_pWeaponServices")
};