#pragma once

#include "ESP.hpp"
#include "ForceCrosshair.hpp"
#include "Menu.hpp"

namespace Features {
	inline ESP esp;
	inline ForceCrosshair forceCrosshair;
	inline Menu menu;

	inline std::initializer_list<Feature* const> allFeatures { &esp, &forceCrosshair, &menu };
}