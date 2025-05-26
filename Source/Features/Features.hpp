#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Feature.hpp"
#include "Setting.hpp"

#include "../Utils/UninitializedObject.hpp"

#include "Misc/ForceCrosshair.hpp"
#include "Visuals/ESP.hpp"

namespace Features {
	inline UninitializedObject<ESP> esp;
	inline UninitializedObject<ForceCrosshair> force_crosshair;

	inline std::vector<std::pair<std::string /*category*/, std::vector<Feature*>>> features;

	void create();
	void destroy();
}
