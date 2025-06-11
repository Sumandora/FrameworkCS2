#pragma once

#include "imgui.h"

namespace ImGuiExt {
	// NOLINTBEGIN(readability-identifier-naming) Mirrors ImGui naming convention
	bool BeginBoxedTabItem(const char* label, const ImVec2& size = ImVec2{ 0, 0 }, bool* p_open = nullptr, ImGuiTabItemFlags flags = 0);
	void EndBoxedTabItem();
	// NOLINTEND(readability-identifier-naming)
}
