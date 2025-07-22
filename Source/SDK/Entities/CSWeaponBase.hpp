#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BasePlayerWeapon.hpp"

#include "../GameClass/GameTick.hpp"

struct CSWeaponBase : public BasePlayerWeapon {
	CLASS_INFO("libclient.so", "C_CSWeaponBase");

	SCHEMA_VAR(GameTick, postpone_fire_ready_ticks, "m_nPostponeFireReadyTicks");
	SCHEMA_VAR(bool, in_reload, "m_bInReload");
};
