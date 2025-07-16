#pragma once

#include "../Padding.hpp"
#include "glm/ext/vector_float3.hpp"

struct ViewSetup {
	OFFSET(glm::vec3, camera_position, 0x4e0);
	OFFSET(glm::vec3, view_angles, 0x4f8);
};
