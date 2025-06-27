#pragma once

#include "../Schema/FieldOffset.hpp"

#include "EntityIdentity.hpp"

struct EntityInstance {
	CLASS_INFO("libclient.so", "CEntityInstance");

	SCHEMA_VAR(EntityIdentity*, entity, "m_pEntity");
};
