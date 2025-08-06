#pragma once

#include "../Columns.hpp"

#include "../../../SDK/ConVar/ConVar.hpp"
#include "../../../SDK/ConVar/EngineCvar.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/GameClass/GameEvent.hpp"

#include "../../../Interfaces.hpp"

#include "imgui.h"

#include <string_view>

// TODO When controlling bots, then the bot is taken to account for the team damage, this is wrong.
struct TeamDamageColumn {
	static constexpr const char* NAME = "Team Damage";
	static constexpr bool HIDDEN_BY_DEFAULT = true;

	static int max_team_damage_before_warn;
	static int max_team_damage_before_kick;

	static void resolve_convars()
	{
		ConVar* mp_td_dmgtowarn = Interfaces::engineCvar->findByName("mp_td_dmgtowarn");
		max_team_damage_before_warn = mp_td_dmgtowarn->get_int();
		ConVar* mp_td_dmgtokick = Interfaces::engineCvar->findByName("mp_td_dmgtokick");
		max_team_damage_before_kick = mp_td_dmgtokick->get_int();
	}

	int damage = 0;

	void update(CSPlayerPawn* pawn, GameEvent* event)
	{
		if (!pawn)
			return;

		if (std::string_view{ event->GetName() } != "player_hurt")
			return;

		BaseEntity* attacker = event->get_entity("attacker");
		if (!attacker || attacker != pawn)
			return;

		BaseEntity* victim = event->get_entity("userid");
		if (!victim)
			return;

		if (pawn->team_id() != victim->team_id())
			return;

		damage += event->get_int("dmg_health", 0);
	}

	void fill() const
	{
		static constexpr ImColor YELLOW{ 1.0F, 1.0F, 0.0F, 1.0F };
		static constexpr ImColor RED{ 1.0F, 0.0F, 0.0F, 1.0F };
		if (damage < max_team_damage_before_warn)
			ImGui::Text("%d/%d", damage, max_team_damage_before_kick);
		else if (damage < max_team_damage_before_kick)
			ImGui::TextColored(YELLOW, "%d/%d", damage, max_team_damage_before_kick);
		else
			ImGui::TextColored(RED, "%d/%d", damage, max_team_damage_before_kick);
	}
};

// This is fine here, because these headers are only included once in the PlayerList.cpp
// NOLINTBEGIN(misc-definitions-in-headers)
int TeamDamageColumn::max_team_damage_before_warn = 0;
int TeamDamageColumn::max_team_damage_before_kick = 0;
// NOLINTEND(misc-definitions-in-headers)

static_assert(Column<TeamDamageColumn>);
