#include "Chams.hpp"

#include "../Feature.hpp"

#include "../../SDK/GameClass/MeshDrawPrimitive.hpp"

#include "imgui.h"

#include <functional>

Chams::Chams()
	: Feature("Visuals", "Chams")
{
}

bool Chams::draw_object(MeshDrawPrimitive* meshes, int count, const std::function<void(MeshDrawPrimitive*, int)>& draw_mesh)
{
	if (!enabled.get())
		return false;

	const ImColor tint_color = tint.get();

	for (int i = 0; i < count; i++) {
		MeshDrawPrimitive& mesh_draw_primitive = meshes[i];
		mesh_draw_primitive.color = tint_color;
	}

	draw_mesh(meshes, count);

	return true;
}
