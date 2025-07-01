#pragma once

#include "../../Schema/FieldOffset.hpp"

#include "BasePlayerWeaponVData.hpp"

struct CSWeaponBaseVData : public BasePlayerWeaponVData {
	CLASS_INFO("libclient.so", "CCSWeaponBaseVData");

	SCHEMA_VAR(int, damage, "m_nDamage");
	SCHEMA_VAR(float, penetration, "m_flPenetration");
	SCHEMA_VAR(float, range, "m_flRange");
	SCHEMA_VAR(float, range_modifier, "m_flRangeModifier");
	SCHEMA_VAR(float, headshot_multiplier, "m_flHeadshotMultiplier");
};
