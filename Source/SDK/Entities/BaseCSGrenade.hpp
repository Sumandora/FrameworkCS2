#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../VirtualMethod.hpp"

#include "BasePlayerWeapon.hpp"

#include "../Enums/GrenadeType.hpp"

#include <string_view>

struct BaseCSGrenade : BasePlayerWeapon {
	CLASS_INFO("libclient.so", "C_BaseCSGrenade");

	SCHEMA_VAR(bool, pin_pulled, "m_bPinPulled");

	VIRTUAL_METHOD(439, get_grenade_type, GrenadeType, (), (this));
};
