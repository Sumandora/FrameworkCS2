#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"
#include "../Settings/Section.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "imgui.h"

#include <unordered_map>

struct SceneLightObject;
struct MeshDrawPrimitive;
struct GameEvent;

class WorldColors : public Feature {
	Section particles{ this, "Particles" };
	Checkbox change_inferno_color{ particles, "Change inferno color", false };
	Color molotov_color{ particles, "Molotov color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };
	Color incendiary_color{ particles, "Incendiary color", ImColor{ 0.0F, 0.0F, 1.0F, 1.0F } };

	Section lights{ this, "Lights" };
	Checkbox change_light_color{ lights, "Change light color", false };
	Color light_color{ lights, "Light color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };

	struct RGB {
		float r, g, b;
	};

	std::unordered_map<SceneLightObject*, RGB> original_light_colors;

public:
	WorldColors();

	void handle_particle(MeshDrawPrimitive* particle_draw_primitives, int count);
	void handle_light(SceneLightObject* scene_light_object);
	void event_handler(GameEvent* event);
};

inline UninitializedObject<WorldColors> world_colors;
