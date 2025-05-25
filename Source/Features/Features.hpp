#pragma once

#include <array>

#include "Feature.hpp"
#include "Setting.hpp"

#include "../Utils/UninitializedObject.hpp"

#include "Misc/ForceCrosshair.hpp"
#include "Visuals/ESP.hpp"

namespace Features {
	inline UninitializedObject<ESP> esp;
	inline UninitializedObject<ForceCrosshair> force_crosshair;

	inline std::array features{
		static_cast<Feature*>(esp.get()),
		static_cast<Feature*>(force_crosshair.get()),
	};

	void create();
	void destroy();
}
