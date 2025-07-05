#include "Projection.hpp"

#include "../Memory.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"

bool Projection::project(const glm::vec3& world, ImVec2& screen)
{
	const glm::mat4x4 matrix = glm::transpose(*Memory::worldToProjectionMatrix);
	const glm::vec4 transformed = matrix * glm::vec4(world, 1.0F);

	if (transformed.z <= 0.0F || transformed.w <= 0.0F)
		return false;

	const float inv_w = 1.0F / transformed.w;
	const float x_norm = transformed.x * inv_w;
	const float y_norm = transformed.y * inv_w;

	const ImVec2 display_size = ImGui::GetIO().DisplaySize;
	screen.x = display_size.x * 0.5F * (1.0F + x_norm);
	screen.y = display_size.y * 0.5F * (1.0F - y_norm);

	return true;
}
