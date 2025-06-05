#pragma once

#include "imgui.h"

namespace ImGuiExt {
	// NOLINTNEXTLINE(readability-identifier-naming) Mirrors ImGui naming convention
	bool BeginResizablePopup(const char* str_id, ImGuiWindowFlags flags = 0);
}
