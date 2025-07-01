#pragma once

#include "../EntityHandle.hpp"
#include "../Schema/FieldOffset.hpp"
#include "BasePlayerController.hpp"
#include "CSPlayerPawn.hpp"

struct CSPlayerController : public BasePlayerController {
	CLASS_INFO("libclient.so", "CCSPlayerController");

	SCHEMA_VAR(const char*, sanitized_name, "m_sSanitizedPlayerName");
	SCHEMA_VAR(EntityHandle<CSPlayerPawn>, player_pawn, "m_hPlayerPawn");
};
