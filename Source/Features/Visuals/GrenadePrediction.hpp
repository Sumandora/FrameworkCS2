#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "ESP/Elements.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

#include <string>

class GrenadePrediction : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Checkbox predict_jumpthrow{ this, "Predict jumpthrow", false };

	PlayerLine line{ this, "Line" };

	glm::vec3 cached_position{};
	glm::vec3 cached_velocity{};

public:
	GrenadePrediction();

	void calculate_grenade_prediction();
	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<GrenadePrediction> grenade_prediction;
