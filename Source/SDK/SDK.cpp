#include "glm/ext/vector_float3.hpp"

#include <cstddef>

using namespace glm;

static_assert(offsetof(vec3, x) == 0);
static_assert(offsetof(vec3, y) == 4);
static_assert(offsetof(vec3, z) == 8);
static_assert(sizeof(vec3) == 12);
