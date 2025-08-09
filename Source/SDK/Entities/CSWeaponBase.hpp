#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BasePlayerWeapon.hpp"

#include "../GameClass/GameTick.hpp"
#include "../GameClass/GameTime.hpp"

#include "../Enums/TeamID.hpp"

struct CSWeaponBase : public BasePlayerWeapon {
	CLASS_INFO("libclient.so", "C_CSWeaponBase");

	SCHEMA_VAR(GameTick, postpone_fire_ready_ticks, "m_nPostponeFireReadyTicks");
	SCHEMA_VAR(bool, in_reload, "m_bInReload");
	SCHEMA_VAR(bool, dropped_near_buy_zone, "m_bDroppedNearBuyZone");
	SCHEMA_VAR(TeamID, most_recent_team_number, "m_iMostRecentTeamNumber");
	SCHEMA_VAR(GameTime, dropped_at_time, "m_flDroppedAtTime");
};
