#pragma once

#include "imgui.h"
#include <mutex>
#include <SDL3/SDL.h>
#include <vector>

namespace GraphicsHook {
	inline std::mutex espMutex;
	inline ImDrawList* espDrawList;
	inline ImDrawListSharedData* espDrawListSharedData;

	void mainLoop();

	bool hookSDL();
	bool hookVulkan();

	void unhookSDL();
	void unhookVulkan();
}
