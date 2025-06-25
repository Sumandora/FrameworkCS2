#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

struct GameRadar;

class Radar : public Feature {
	Checkbox enabled{ this, "Enabled", false };

public:
	Radar();

	void on_radar_update(GameRadar* radar);
};

inline UninitializedObject<Radar> radar;
