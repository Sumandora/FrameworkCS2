#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/TextInput.hpp"

#include "../../Utils/UninitializedObject.hpp"

class ModelChanger : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	TextInput model_path{ this, "Model path", "characters/models/ctm_diver/ctm_diver_varianta.vmdl" }; // TODO Combo??? this is a meme

public:
	ModelChanger();

	void update_model();
};

inline UninitializedObject<ModelChanger> model_changer;
