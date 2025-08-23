#pragma once

#include "../Schema/FieldOffset.hpp"
#include "BaseModelEntity.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstdint>

struct Inferno : public BaseModelEntity {
	CLASS_INFO("libclient.so", "C_Inferno");

	SCHEMA_VAR(glm::vec3[64], fire_positions, "m_firePositions");
	SCHEMA_VAR(glm::vec3[64], fire_parent_positions, "m_fireParentPositions");
	SCHEMA_VAR(bool[64], fire_is_burning, "m_bFireIsBurning");
	SCHEMA_VAR(std::int32_t, fire_count, "m_fireCount");
	SCHEMA_VAR(std::int32_t, inferno_type, "m_nInfernoType");
	SCHEMA_VAR(glm::vec3, min_bounds, "m_minBounds");
	SCHEMA_VAR(glm::vec3, max_bounds, "m_maxBounds");
};
