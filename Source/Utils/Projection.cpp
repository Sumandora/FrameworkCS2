#include "Projection.hpp"

#include "../Memory.hpp"
#include "../SDK/Math/Matrix.hpp"
#include "../SDK/Math/Vector.hpp"
#include "imgui.h"

bool project(const Vector3& world, ImVec2& screen)
{
	const VMatrix& matrix = *Memory::worldToProjectionMatrix;

	const float z = matrix[2][0] * world[0] + matrix[2][1] * world[1] + matrix[2][2] * world[2] + matrix[2][3];
	const float w = matrix[3][0] * world[0] + matrix[3][1] * world[1] + matrix[3][2] * world[2] + matrix[3][3];
	if (z <= 0.0f || w <= 0.0f)
		return false;

	screen = ImGui::GetIO().DisplaySize;
	screen.x *= 0.5f;
	screen.y *= 0.5f;

	screen.x *= 1.0f + (matrix[0][0] * world[0] + matrix[0][1] * world[1] + matrix[0][2] * world[2] + matrix[0][3]) / w;
	screen.y *= 1.0f - (matrix[1][0] * world[0] + matrix[1][1] * world[1] + matrix[1][2] * world[2] + matrix[1][3]) / w;
	return true;
}
