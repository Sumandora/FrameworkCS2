#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "imgui.h"

struct BaseEntity;
struct GlowProperty;

class Glow : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Color color{ this, "Color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };

public:
	Glow();

	bool wants_modify(BaseEntity* entity);
	ImColor get_color_for_entity(BaseEntity* entity);
};

inline UninitializedObject<Glow> glow;
