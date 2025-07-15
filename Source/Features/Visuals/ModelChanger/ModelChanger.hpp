#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Checkbox.hpp"

#include "../../../Utils/UninitializedObject.hpp"

#include "PlayerModelCombo.hpp"

#include <string>

class ModelChanger : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	PlayerModelCombo model{ this, "Model" };

public:
	ModelChanger();

	void update_model();
};

inline UninitializedObject<ModelChanger> model_changer;
