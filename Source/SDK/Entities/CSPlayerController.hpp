#pragma once

#include "../VirtualMethod.hpp"

#include "../EntityHandle.hpp"

#include "../Schema/FieldOffset.hpp"

#include "BasePlayerController.hpp"
#include "CSObserverPawn.hpp"
#include "CSPlayerPawn.hpp"
#include "Services/InGameMoneyServices.hpp"

#include <cstdint>
#include <string>

// Refer to the implementation of the "GetPlayerCompetitiveRankType" panorama function.
enum CompetitiveRankType : std::uint8_t {
	RANKTYPE_WINGMAN = 7,
	RANKTYPE_SCRIMCOMP_2V2_2017 = 8,
	RANKTYPE_SCRIMCOMP_5V5_2017 = 9,
	RANKTYPE_DANGERZONE = 10,
	RANKTYPE_PREMIER = 0xb,
	RANKTYPE_COMPETITIVE = 0xc,
};

struct CSPlayerController : public BasePlayerController {
	CLASS_INFO("libclient.so", "CCSPlayerController");

	SCHEMA_VAR(InGameMoneyServices*, in_game_money_services, "m_pInGameMoneyServices");
	SCHEMA_VAR(std::uint32_t, ping, "m_iPing");
	SCHEMA_VAR(const char*, sanitized_name, "m_sSanitizedPlayerName");
	SCHEMA_VAR(std::int32_t, competitive_ranking, "m_iCompetitiveRanking");
	SCHEMA_VAR(std::int32_t, competitive_wins, "m_iCompetitiveWins");
	SCHEMA_VAR(std::int8_t, competitive_rank_type, "m_iCompetitiveRankType");
	SCHEMA_VAR(std::int32_t, competitive_ranking_predicted_win, "m_iCompetitiveRankingPredicted_Win");
	SCHEMA_VAR(std::int32_t, competitive_ranking_predicted_loss, "m_iCompetitiveRankingPredicted_Loss");
	SCHEMA_VAR(std::int32_t, competitive_ranking_predicted_tie, "m_iCompetitiveRankingPredicted_Tie");
	SCHEMA_VAR(bool, is_controlling_bot, "m_bControllingBot");
	SCHEMA_VAR(EntityHandle<CSPlayerPawn>, player_pawn, "m_hPlayerPawn");
	SCHEMA_VAR(EntityHandle<CSObserverPawn>, observer_pawn, "m_hObserverPawn");
	SCHEMA_VAR(bool, pawn_is_alive, "m_bPawnIsAlive");

	VIRTUAL_METHOD(8, get_player_name, const char*, (int index), (this, index));

	static void resolve_signatures();

	[[nodiscard]] std::string get_decorated_player_name() const;
};
