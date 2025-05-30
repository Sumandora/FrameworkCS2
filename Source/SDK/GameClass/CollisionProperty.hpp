#pragma once

#include "../Schema/FieldOffset.hpp"

#include "glm/ext/vector_float3.hpp"

struct CollisionProperty {
	CLASS_INFO("libclient.so", "CCollisionProperty");

	SCHEMA_VAR(glm::vec3, mins, "m_vecMins");
	SCHEMA_VAR(glm::vec3, maxs, "m_vecMaxs");
};
