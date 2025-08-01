#pragma once

namespace GraphicsHook {
	bool hookSDL();
	bool hookVulkan();

	void unhookSDL();
	void unhookVulkan();
}
