#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include <memory>
#include <mutex>

namespace GraphicsHook {
	inline std::mutex espMutex;
	inline std::unique_ptr<ImDrawList> espDrawList;
	inline std::unique_ptr<ImDrawListSharedData> espDrawListSharedData;

	bool hookSDL();
	bool hookVulkan();

	void unhookSDL();
	void unhookVulkan();
}
