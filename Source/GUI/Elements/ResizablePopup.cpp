#include "ResizablePopup.hpp"

#include "imgui.h"
#include "imgui_internal.h"

// This function is basically just ImGui::BeginPopup

bool ImGuiExt::BeginResizablePopup(const char* str_id, ImGuiWindowFlags flags)
{
	ImGuiContext& g = *GImGui;
	if (g.OpenPopupStack.Size <= g.BeginPopupStack.Size) {
		g.NextWindowData.ClearFlags();
		return false;
	}
	flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings;
	const ImGuiID id = g.CurrentWindow->GetID(str_id);
	return ImGui::BeginPopupEx(id, flags);
}
