#pragma once

#include "imgui.h"
#include <functional>
#include <mutex>
#include <SDL3/SDL.h>
#include <vector>

namespace GraphicsHook {
	inline std::mutex eventAccessMutex; // https://github.com/ocornut/imgui/issues/6895
	inline std::vector<SDL_Event> eventQueue;

	inline std::mutex espMutex;
	inline ImDrawList* espDrawList;
	inline ImDrawListSharedData* espDrawListSharedData;

	void mainLoop();

	bool hookSDL();
	bool hookVulkan();

	void unhookSDL();
	void unhookVulkan();
}