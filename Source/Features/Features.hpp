#pragma once

#include "imgui.h"

namespace Features {
	namespace ESP {
		inline bool enabled = false;

		void drawEsp(ImDrawList* drawList);
		void imguiRender();
		void update();
	}
	namespace ForceCrosshair {
		inline bool enabled = false;

		bool shouldForce();
		void imguiRender();
	}
}