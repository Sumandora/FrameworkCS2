#pragma once

#include "../../GameClass/GameTick.hpp"
#include "../../Schema/FieldOffset.hpp"
#include "../../Schema/SchemaClassInfo.hpp"
#include "EconEntity.hpp"

struct BasePlayerWeapon : EconEntity {
	CLASS_INFO("libclient.so", "C_BasePlayerWeapon");

	SCHEMA_VAR(std::int32_t, clip1, "m_iClip1");
	SCHEMA_VAR(GameTick, nextPrimaryAttackTick, "m_nNextPrimaryAttackTick");
};