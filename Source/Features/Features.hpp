#pragma once

#include "imgui.h"
#include <string>

namespace Features {
	namespace ESP {
		inline bool enabled = false;

		void drawEsp(ImDrawList* drawList);
		void imguiRender();
	}
	namespace ForceCrosshair {
		inline bool enabled = false;

		bool shouldForce();
		void imguiRender();
	}
	namespace LoadingTextChanger {
		inline bool enabled = false;
		inline std::string replacement = "There I am, Gary!";

		const char* getReplacement();
		void imguiRender();
	}
}