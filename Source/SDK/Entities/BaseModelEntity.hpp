#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BaseEntity.hpp"

#include "glm/ext/vector_float3.hpp"

struct BaseModelEntity : public BaseEntity {
	CLASS_INFO("libclient.so", "C_BaseModelEntity");

	SCHEMA_VAR(glm::vec3, view_offset, "m_vecViewOffset")
};
