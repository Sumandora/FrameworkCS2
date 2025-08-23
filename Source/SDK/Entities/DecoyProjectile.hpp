#pragma once

#include "../Schema/FieldOffset.hpp"
#include "BaseCSGrenadeProjectile.hpp"

struct DecoyProjectile : public BaseCSGrenadeProjectile {
	CLASS_INFO("libclient.so", "C_DecoyProjectile");
};
