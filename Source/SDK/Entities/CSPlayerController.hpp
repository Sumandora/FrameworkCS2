#pragma once

#include "../VirtualMethod.hpp"

#include "../EntityHandle.hpp"
#include "../Schema/FieldOffset.hpp"
#include "BasePlayerController.hpp"
#include "CSObserverPawn.hpp"
#include "CSPlayerPawn.hpp"

#include <string>

struct CSPlayerController : public BasePlayerController {
	CLASS_INFO("libclient.so", "CCSPlayerController");

	SCHEMA_VAR(const char*, sanitized_name, "m_sSanitizedPlayerName");
	SCHEMA_VAR(EntityHandle<CSPlayerPawn>, player_pawn, "m_hPlayerPawn");
	SCHEMA_VAR(EntityHandle<CSObserverPawn>, observer_pawn, "m_hObserverPawn");
	SCHEMA_VAR(bool, pawn_is_alive, "m_bPawnIsAlive");
	SCHEMA_VAR(bool, is_controlling_bot, "m_bControllingBot");

	VIRTUAL_METHOD(8, get_player_name, const char*, (int index), (this, index));

	static void resolve_signatures();

	[[nodiscard]] std::string get_decorated_player_name() const;
};
