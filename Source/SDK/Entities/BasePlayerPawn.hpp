#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BaseEntity.hpp"

#include "glm/ext/vector_float3.hpp"

struct BasePlayerPawn : public BaseEntity {
	CLASS_INFO("libclient.so", "C_BasePlayerPawn");

	SCHEMA_VAR(glm::vec3, old_origin, "m_vOldOrigin")
};
