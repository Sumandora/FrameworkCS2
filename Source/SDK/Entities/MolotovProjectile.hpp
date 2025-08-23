#pragma once

#include "../Schema/FieldOffset.hpp"
#include "BaseCSGrenadeProjectile.hpp"

struct MolotovProjectile : public BaseCSGrenadeProjectile {
	CLASS_INFO("libclient.so", "C_MolotovProjectile");

	SCHEMA_VAR(bool, is_inc_grenade, "m_bIsIncGrenade");
};
