#include "Spacing.hpp"

#include "../../Config/Config.hpp"

#include <imgui.h>

void Menu::addSpacing(const int pixels)
{
	const float scaled = static_cast<float>(pixels) * Config::getDpiScale();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, scaled });
	ImGui::Spacing();
	ImGui::PopStyleVar();
}