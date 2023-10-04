#pragma once

#include <mutex>

namespace GraphicsHook {
	inline std::mutex eventAccessMutex; // https://github.com/ocornut/imgui/issues/6895

	bool hookSDL();
	bool hookVulkan();

	void unhookSDL();
	void unhookVulkan();
}