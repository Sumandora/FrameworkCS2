#pragma once

#include "../Math/Vector.hpp"
#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"

struct CollisionProperty {
	CLASS_INFO("libclient.so", "CCollisionProperty");

	SCHEMA_VAR(Vector3, mins, "m_vecMins");
	SCHEMA_VAR(Vector3, maxs, "m_vecMaxs");
};