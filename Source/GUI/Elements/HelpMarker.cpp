#include "HelpMarker.hpp"

#include "imgui.h"

// Taken from the imgui demo

void ImGuiExt::HelpMarker(const char* desc)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0F);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
