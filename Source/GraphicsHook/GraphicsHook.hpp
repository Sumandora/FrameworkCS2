#pragma once

#include <mutex>
#include <vector>
#include <SDL3/SDL.h>

namespace GraphicsHook {
	inline std::mutex eventAccessMutex; // https://github.com/ocornut/imgui/issues/6895
	inline std::vector<SDL_Event> eventQueue;

	bool hookSDL();
	bool hookVulkan();

	void unhookSDL();
	void unhookVulkan();
}