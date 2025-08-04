#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

class GameEvent;

class BulletTracers : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Color color{ this, "Color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };
	FloatSlider lifetime{ this, "Lifetime", 0.0F, 10.0F, 1.0F };
	FloatSlider width{ this, "Width", 0.0F, 10.0F, 2.0F };

	glm::vec3 view_angles;

public:
	BulletTracers();

	void update_viewangles(const glm::vec3& view_angles);

	void event_handler(GameEvent* event);
};

inline UninitializedObject<BulletTracers> bullet_tracers;
