#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Feature.hpp"

#include "../Utils/UninitializedObject.hpp"

#include "Visuals/ESP/ESP.hpp"
#include "Visuals/ForceCrosshair.hpp"
#include "Visuals/LoadingTextChanger.hpp"

namespace Features {
	inline UninitializedObject<ESP> esp;
	inline UninitializedObject<ForceCrosshair> force_crosshair;
	inline UninitializedObject<LoadingTextChanger> loading_text_changer;

	inline std::vector<std::pair<std::string /*category*/, std::vector<Feature*>>> features;

	void create();
	void destroy();
}
