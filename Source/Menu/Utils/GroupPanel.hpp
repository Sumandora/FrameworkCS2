#pragma once

#include <imgui.h>

namespace Menu::GroupPanel {
	void Begin(const char* name, const ImVec2& size = ImVec2(-1.0f, 0.0f));
	void End();
}