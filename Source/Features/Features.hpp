#pragma once

#include "imgui.h"

#include <array>
#include <memory>
#include <tuple>
#include <vector>

#include "Feature.hpp"
#include "Setting.hpp"

#include "../Utils/UninitializedObject.hpp"

class ESP : public Feature {
	Checkbox enabled = make_checkbox("Enabled");

public:
	ESP();

	void draw(ImDrawList* draw_list);
};

namespace Features {
	inline UninitializedObject<ESP> esp;

	inline std::array features{
		static_cast<Feature*>(esp.get()),
	};

	void create();
	void destroy();
}

namespace Features {
	namespace ForceCrosshair {
		inline bool enabled = false;

		bool shouldForce();
		void imguiRender();
	}
}
