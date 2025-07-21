#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

struct ImVec2;

namespace Projection {
	void resolve_signatures();

	bool project(const glm::vec3& world, ImVec2& screen);

	glm::mat4x4* get_world_to_projection_matrix();
}
