#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaSystem.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstddef>
#include <cstdint>

struct CollisionProperty {
	CLASS_INFO("libclient.so", "CCollisionProperty");

	SCHEMA_VAR(glm::vec3, mins, "m_vecMins");
	SCHEMA_VAR(glm::vec3, maxs, "m_vecMaxs");

	std::uint16_t collision_mask()
	{
		static const std::int32_t OFFSET = SchemaSystem::findOffset(classInfo(), "m_vecMins");
		return *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::byte*>(this) + OFFSET - 0x8);
	}
};
