#pragma once

#include "glm/ext/vector_float3.hpp"

struct ImVec2;

namespace Projection {
	bool project(const glm::vec3& world, ImVec2& screen);
}
