#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/GameClass/GameEvent.hpp"

#include "imgui.h"

#include <string_view>

// TODO When controlling bots, then the bot is taken to account for the team damage, this is wrong.
struct TeamKillsColumn {
	static constexpr const char* NAME = "Team Kills";
	static constexpr bool HIDDEN_BY_DEFAULT = true;

	// TODO is this something the game determines dynamically? Because there is no convar for it...
	static constexpr int MAX_TEAM_KILLS_BEFORE_KICK = 3;

	int kills = 0;

	void update(CSPlayerPawn* pawn, GameEvent* event)
	{
		if (!pawn)
			return;

		if (std::string_view{ event->GetName() } != "player_death")
			return;

		BaseEntity* attacker = event->get_entity("attacker");
		if (!attacker || attacker != pawn)
			return;

		BaseEntity* victim = event->get_entity("userid");
		if (!victim)
			return;

		if (pawn->team_id() != victim->team_id())
			return;

		kills++;
	}

	void fill() const
	{
		static constexpr ImColor RED{ 1.0F, 0.0F, 0.0F, 1.0F };
		if (kills < MAX_TEAM_KILLS_BEFORE_KICK)
			ImGui::Text("%d/%d", kills, MAX_TEAM_KILLS_BEFORE_KICK);
		else
			ImGui::TextColored(RED, "%d/%d", kills, MAX_TEAM_KILLS_BEFORE_KICK);
	}
};

static_assert(Column<TeamKillsColumn>);
