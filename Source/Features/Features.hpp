#pragma once

#include <string>
#include <utility>
#include <vector>

#include "Feature.hpp"

namespace Features {
	inline std::vector<std::pair<std::string /*category*/, std::vector<Feature*>>> features;

	void create();
	void destroy();
}
