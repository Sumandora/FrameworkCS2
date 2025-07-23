#include "BoxedTabItem.hpp"
#include "imgui.h"

bool ImGuiExt::BeginBoxedTabItem(const char* label, const ImVec2& size, bool* p_open, ImGuiTabItemFlags flags)
{
	if (ImGui::BeginTabItem(label, p_open, flags)) {
		if (!ImGui::BeginChild(label, size, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize)) {
			EndBoxedTabItem();
			return false;
		}
		return true;
	}
	return false;
}

void ImGuiExt::EndBoxedTabItem()
{
	ImGui::EndChild();
	ImGui::EndTabItem();
}
