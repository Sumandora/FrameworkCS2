#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

#include "octree-cpp/OctreeCpp.h"

#include <cctype>
#include <string>

struct GameEvent;

class GrenadeHelper : public Feature {

	Checkbox enabled{ this, "Enabled", false };
	FloatSlider render_distance{ this, "Render distance", 0.0F, 32768.0F, 4096.0F };

	using Octree = OctreeCpp<glm::vec3, Serialization::Grenades::Grenade>;
	Octree grenades;
	std::string current_map;

public:
	GrenadeHelper();

	void event_handler(GameEvent* event);
	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<GrenadeHelper> grenade_helper;
