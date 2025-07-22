#pragma once

#include "../../Schema/FieldOffset.hpp"

#include "BasePlayerWeaponVData.hpp"

#include <cstdint>

// NOLINTNEXTLINE(performance-enum-size)
enum CSWeaponType : std::uint32_t {
	WEAPONTYPE_KNIFE = 0x0,
	WEAPONTYPE_PISTOL = 0x1,
	WEAPONTYPE_SUBMACHINEGUN = 0x2,
	WEAPONTYPE_RIFLE = 0x3,
	WEAPONTYPE_SHOTGUN = 0x4,
	WEAPONTYPE_SNIPER_RIFLE = 0x5,
	WEAPONTYPE_MACHINEGUN = 0x6,
	WEAPONTYPE_C4 = 0x7,
	WEAPONTYPE_TASER = 0x8,
	WEAPONTYPE_GRENADE = 0x9,
	WEAPONTYPE_EQUIPMENT = 0xa,
	WEAPONTYPE_STACKABLEITEM = 0xb,
	WEAPONTYPE_FISTS = 0xc,
	WEAPONTYPE_BREACHCHARGE = 0xd,
	WEAPONTYPE_BUMPMINE = 0xe,
	WEAPONTYPE_TABLET = 0xf,
	WEAPONTYPE_MELEE = 0x10,
	WEAPONTYPE_SHIELD = 0x11,
	WEAPONTYPE_ZONE_REPULSOR = 0x12,
	WEAPONTYPE_UNKNOWN = 0x13,
};

struct CSWeaponBaseVData : public BasePlayerWeaponVData {
	CLASS_INFO("libclient.so", "CCSWeaponBaseVData");

	SCHEMA_VAR(CSWeaponType, weapon_type, "m_WeaponType");

	SCHEMA_VAR(int, damage, "m_nDamage");
	SCHEMA_VAR(float, penetration, "m_flPenetration");
	SCHEMA_VAR(float, range, "m_flRange");
	SCHEMA_VAR(float, range_modifier, "m_flRangeModifier");
	SCHEMA_VAR(float, headshot_multiplier, "m_flHeadshotMultiplier");
	SCHEMA_VAR(float, armor_ratio, "m_flArmorRatio");

	[[nodiscard]] constexpr bool is_gun() const
	{
		const CSWeaponType type = weapon_type();
		return type >= WEAPONTYPE_PISTOL && type <= WEAPONTYPE_MACHINEGUN;
	}
};
