#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatDrag.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"
#include "glm/ext/vector_float3.hpp"
#include <optional>

struct ViewSetup;

class FieldOfView : public Feature {
	Checkbox change_fov{ this, "Change fov", false };
	FloatSlider fov{ this, "FOV", 0.0F, 180.0F, 90.0F };
	Checkbox ignore_scope{ this, "Ignore scope", true };

	Checkbox change_viewmodel{ this, "Change viewmodel", false };
	FloatSlider viewmodel_fov{ this, "Viewmodel FOV", 0.0F, 180.0F, 68.0F };
	FloatDrag x_offset{ this, "X offset", 0.1F, 0.0F };
	FloatDrag y_offset{ this, "Y offset", 0.1F, 0.0F };
	FloatDrag z_offset{ this, "Z offset", 0.1F, 0.0F };
	// TODO Rotation

public:
	FieldOfView();

	std::optional<float> get_fov();

	bool modifies_viewmodel();
	void modify_viewmodel_fov(float* fov);
	void modify_viewmodel_position(glm::vec3* position);
	void modify_viewmodel_rotation(glm::vec3* rotation);
};

inline UninitializedObject<FieldOfView> field_of_view;
