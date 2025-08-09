#pragma once

#include "EconEntity.hpp"

#include "../Schema/FieldOffset.hpp"

#include "../GameClass/GameTick.hpp"

#include <cstdint>

struct BasePlayerWeapon : public EconEntity {
	CLASS_INFO("libclient.so", "C_BasePlayerWeapon");

	SCHEMA_VAR(GameTick, next_primary_attack_tick,"m_nNextPrimaryAttackTick");
	SCHEMA_VAR(GameTick, next_secondary_attack_tick,"m_nNextSecondaryAttackTick");
	SCHEMA_VAR(int32_t, clip1,"m_iClip1");
	SCHEMA_VAR(int32_t, clip2,"m_iClip2");
};
