#include "FieldOfView.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/CSPlayerPawn.hpp"

#include "../../Memory.hpp"
#include "glm/ext/vector_float3.hpp"

#include <optional>

FieldOfView::FieldOfView()
	: Feature("Visuals", "Field of view")
{
	fov.add_visible_condition([this] { return change_fov.get_raw(); });
	ignore_scope.add_visible_condition([this] { return change_fov.get_raw(); });

	viewmodel_fov.add_visible_condition([this] { return change_viewmodel.get_raw(); });
	x_offset.add_visible_condition([this] { return change_viewmodel.get_raw(); });
	y_offset.add_visible_condition([this] { return change_viewmodel.get_raw(); });
	z_offset.add_visible_condition([this] { return change_viewmodel.get_raw(); });
}

std::optional<float> FieldOfView::get_fov()
{
	if (!Memory::local_player)
		return {};

	if (!change_fov.get())
		return {};

	if (Memory::local_player->is_scoped() && ignore_scope.get())
		return {};

	return fov.get();
}

bool FieldOfView::modifies_viewmodel()
{
	return change_viewmodel.get();
}

void FieldOfView::modify_viewmodel_fov(float* fov)
{
	*fov = viewmodel_fov.get();
}

void FieldOfView::modify_viewmodel_position(glm::vec3* position)
{
	position->x += x_offset.get();
	position->y += y_offset.get();
	position->z += z_offset.get();
}
