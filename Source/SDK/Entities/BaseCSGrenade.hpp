#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BasePlayerWeapon.hpp"

#include "../VirtualMethod.hpp"
#include <cstdint>

// @schema GrenadeType_t
// NOLINTNEXTLINE(performance-enum-size)
enum GrenadeType : std::uint32_t {
	GRENADE_TYPE_EXPLOSIVE = 0,
	GRENADE_TYPE_FLASH = 1,
	GRENADE_TYPE_FIRE = 2,
	GRENADE_TYPE_DECOY = 3,
	GRENADE_TYPE_SMOKE = 4,
	GRENADE_TYPE_SENSOR = 5,
	GRENADE_TYPE_SNOWBALL = 6,
	GRENADE_TYPE_TOTAL = 7,
};

struct BaseCSGrenade : BasePlayerWeapon {
	CLASS_INFO("libclient.so", "C_BaseCSGrenade");

	SCHEMA_VAR(bool, pin_pulled, "m_bPinPulled")

	VIRTUAL_METHOD(438, get_grenade_type, GrenadeType, (), (this));
};
