#pragma once

#include "../SDK/Math/Vector.hpp"

struct ImVec2;

namespace Projection {
	bool project(const Vector3& world, ImVec2& screen);
}