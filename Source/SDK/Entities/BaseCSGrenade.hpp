#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BasePlayerWeapon.hpp"

#include "../VirtualMethod.hpp"

struct BaseCSGrenade : BasePlayerWeapon {
	CLASS_INFO("libclient.so", "BaseCSGrenade");

	VIRTUAL_METHOD(438, get_grenade_type, int, (), (this));
};
