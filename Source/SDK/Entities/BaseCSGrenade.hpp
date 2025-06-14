#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BasePlayerWeapon.hpp"

#include "../VirtualMethod.hpp"

struct BaseCSGrenade : BasePlayerWeapon {
	CLASS_INFO("libclient.so", "C_BaseCSGrenade");

	SCHEMA_VAR(bool, pin_pulled, "m_bPinPulled")

	VIRTUAL_METHOD(438, get_grenade_type, int, (), (this));
};
