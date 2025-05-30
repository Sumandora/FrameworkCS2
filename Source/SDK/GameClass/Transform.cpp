#include "Transform.hpp"
#include "glm/ext/matrix_float3x4.hpp"

glm::mat3x4 Transform::toMatrix() const
{
	glm::mat3x4 matrix{};

	const auto& rot = m_Orientation;
	const auto& pos = m_Position;

	matrix[0][0] = 1.0f - 2.0f * rot[1] * rot[1] - 2.0f * rot[2] * rot[2];
	matrix[1][0] = 2.0f * rot[0] * rot[1] + 2.0f * rot[3] * rot[2];
	matrix[2][0] = 2.0f * rot[0] * rot[2] - 2.0f * rot[3] * rot[1];

	matrix[0][1] = 2.0f * rot[0] * rot[1] - 2.0f * rot[3] * rot[2];
	matrix[1][1] = 1.0f - 2.0f * rot[0] * rot[0] - 2.0f * rot[2] * rot[2];
	matrix[2][1] = 2.0f * rot[1] * rot[2] + 2.0f * rot[3] * rot[0];

	matrix[0][2] = 2.0f * rot[0] * rot[2] + 2.0f * rot[3] * rot[1];
	matrix[1][2] = 2.0f * rot[1] * rot[2] - 2.0f * rot[3] * rot[0];
	matrix[2][2] = 1.0f - 2.0f * rot[0] * rot[0] - 2.0f * rot[1] * rot[1];

	matrix[0][3] = pos[0];
	matrix[1][3] = pos[1];
	matrix[2][3] = pos[2];

	return matrix;
}
