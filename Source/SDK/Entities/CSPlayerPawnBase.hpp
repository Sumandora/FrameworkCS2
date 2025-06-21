#pragma once

#include "BasePlayerPawn.hpp"
#include "../EntityHandle.hpp"

struct CSPlayerController;
struct PlayerWeaponServices;

struct CSPlayerPawnBase : public BasePlayerPawn {
	CLASS_INFO("libclient.so", "C_CSPlayerPawnBase");

	SCHEMA_VAR(EntityHandle<CSPlayerController>, original_controller, "m_hOriginalController")

    SCHEMA_VAR(PlayerWeaponServices*, weapon_services, "m_pWeaponServices")
};
