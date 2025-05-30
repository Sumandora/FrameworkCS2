#pragma once

#include "glm/ext/matrix_float3x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

class alignas(16) Transform {
public:
	alignas(16) glm::vec3 m_Position;
	alignas(16) glm::vec4 m_Orientation;

	[[nodiscard]] glm::mat3x4 toMatrix() const;
};
