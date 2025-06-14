#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../EntityHandle.hpp"
#include "CSPlayerPawn.hpp"
#include "BasePlayerController.hpp"

struct CSPlayerController : BasePlayerController {
	CLASS_INFO("libclient.so", "CCSPlayerController");

	SCHEMA_VAR(const char*, sanitizied_name, "m_sSanitizedPlayerName");
	SCHEMA_VAR(EntityHandle<CSPlayerPawn>, player_pawn, "m_hPlayerPawn")
};
