#pragma once

#include "../Schema/FieldOffset.hpp"

struct EntityIdentity;

struct EntityInstance {
	CLASS_INFO("libclient.so", "CEntityInstance");

	SCHEMA_VAR(EntityIdentity*, entity, "m_pEntity");
};
