#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"
#include "glm/ext/vector_float3.hpp"

struct ViewSetup;

class ForceThirdPerson : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	FloatSlider distance{ this, "Distance", 0.0F, 300.0F, 150.0F };

	glm::vec3 view_angles;

public:
	ForceThirdPerson();

	void update_viewangles(const glm::vec3& view_angles);

	void override_view(ViewSetup* view_setup);
};

inline UninitializedObject<ForceThirdPerson> force_third_person;
