#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "imgui.h"

#include <functional>

struct MeshDrawPrimitive;

class Chams : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Color tint{ this, "Tint color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };

public:
	Chams();

	// @returns true if original should be canceled
	bool draw_object(MeshDrawPrimitive* meshes, int count, const std::function<void(MeshDrawPrimitive*, int)>& draw_mesh);
};

inline UninitializedObject<Chams> chams;
