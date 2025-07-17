#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../EntityHandle.hpp"

#include "BasePlayerPawn.hpp"

#include "Services/CSPlayerItemServices.hpp"
#include "Services/CSPlayerMovementServices.hpp"


struct CSPlayerController;

struct CSPlayerPawnBase : public BasePlayerPawn {
	CLASS_INFO("libclient.so", "C_CSPlayerPawnBase");

	SCHEMA_VAR(EntityHandle<CSPlayerController>, original_controller, "m_hOriginalController");

	SCHEMA_VAR(bool, gun_game_immunity, "m_bGunGameImmunity");

	[[nodiscard]] CSPlayerMovementServices* movement_services() const
	{
		return static_cast<CSPlayerMovementServices*>(BasePlayerPawn::movement_services());
	}
	[[nodiscard]] CSPlayerItemServices* item_services() const
	{
		return static_cast<CSPlayerItemServices*>(BasePlayerPawn::item_services());
	}
};
