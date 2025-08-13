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

	static ConVar* mp_td_dmgtowarn;
	static ConVar* mp_td_dmgtokick;

	static void resolve_convars()
	{
		mp_td_dmgtowarn = Interfaces::engineCvar->findByName("mp_td_dmgtowarn");
		mp_td_dmgtokick = Interfaces::engineCvar->findByName("mp_td_dmgtokick");
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
		if (!victim || victim == attacker)
			return;

		if (pawn->team_id() != victim->team_id())
			return;

		damage += event->get_int("dmg_health", 0);
	}

	void fill() const
	{
		static constexpr ImColor YELLOW{ 1.0F, 1.0F, 0.0F, 1.0F };
		static constexpr ImColor RED{ 1.0F, 0.0F, 0.0F, 1.0F };
		const int damage_before_kick = mp_td_dmgtokick->get_int();
		if (damage < mp_td_dmgtowarn->get_int())
			ImGui::Text("%d/%d", damage, damage_before_kick);
		else if (damage < damage_before_kick)
			ImGui::TextColored(YELLOW, "%d/%d", damage, damage_before_kick);
		else
			ImGui::TextColored(RED, "%d/%d", damage, damage_before_kick);
	}
};

// This is fine here, because these headers are only included once in the PlayerList.cpp
// NOLINTBEGIN(misc-definitions-in-headers)
ConVar* TeamDamageColumn::mp_td_dmgtowarn = nullptr;
ConVar* TeamDamageColumn::mp_td_dmgtokick = nullptr;
// NOLINTEND(misc-definitions-in-headers)

static_assert(Column<TeamDamageColumn>);
