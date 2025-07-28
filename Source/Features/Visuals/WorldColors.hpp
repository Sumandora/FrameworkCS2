#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"
#include "../Settings/Section.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "imgui.h"

struct MeshDrawPrimitive;

class WorldColors : public Feature {
	Section particles{ this, "Particles" };
	Checkbox change_inferno_color{ particles, "Change inferno color", false };
	Color molotov_color{ particles, "Molotov color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };
	Color incendiary_color{ particles, "Incendiary color", ImColor{ 0.0F, 0.0F, 1.0F, 1.0F } };

public:
	WorldColors();

	void handle_particle(MeshDrawPrimitive* particle_draw_primitives, int count);
};

inline UninitializedObject<WorldColors> world_colors;
