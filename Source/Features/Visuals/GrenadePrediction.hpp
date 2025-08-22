#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "ESP/Elements.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

#include <mutex>
#include <string>
#include <vector>

class GrenadePrediction : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Checkbox predict_jumpthrow{ this, "Predict jumpthrow", false };

	PlayerLine line{ this, "Line" };

	std::vector<glm::vec3> points;
	mutable std::mutex mutex;

public:
	GrenadePrediction();

	void calculate_grenade_prediction();
	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<GrenadePrediction> grenade_prediction;
