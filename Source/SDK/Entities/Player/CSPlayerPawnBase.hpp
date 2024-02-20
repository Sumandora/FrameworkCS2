#pragma once

#include "BasePlayerPawn.hpp"
#include "CSPlayerController.hpp"

struct CSPlayerWeaponServices;

struct CSPlayerPawnBase : BasePlayerPawn {
	CLASS_INFO("libclient.so", "C_CSPlayerPawnBase");

	SCHEMA_VAR(bool, isScoped, "m_bIsScoped");
	SCHEMA_VAR(std::int32_t, armorValue, "m_ArmorValue");
	SCHEMA_VAR(Handle<CSPlayerController>, originalController, "m_hOriginalController");

	[[nodiscard]] CSPlayerWeaponServices* weaponServices() const;
};