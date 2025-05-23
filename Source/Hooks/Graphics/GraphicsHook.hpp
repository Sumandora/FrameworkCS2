#pragma once

#include "SDL3/SDL_video.h"
#include "imgui.h"
#include <mutex>
#include <SDL3/SDL.h>
#include <vector>

namespace GraphicsHook {
	inline std::mutex espMutex;
	inline ImDrawList* espDrawList;
	inline ImDrawListSharedData* espDrawListSharedData;

	bool hookSDL();
	bool hookVulkan();

	void unhookSDL();
	void unhookVulkan();
}
