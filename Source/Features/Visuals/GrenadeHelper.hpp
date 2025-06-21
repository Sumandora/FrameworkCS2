#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "imgui.h"

#include <cctype>
#include <string>
#include <vector>

struct GameEvent;

class GrenadeHelper : public Feature {

	Checkbox enabled{ this, "Enabled", false };

	std::vector<Serialization::Grenades::Grenade> grenades;
	std::string current_map;

public:
	GrenadeHelper();

	void event_handler(GameEvent* event);
	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<GrenadeHelper> grenade_helper;
