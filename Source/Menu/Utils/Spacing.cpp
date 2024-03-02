#include "Spacing.hpp"

#include "../../Features/Features.hpp"

#include <imgui.h>

namespace Menu {
	void addSpacing(const float pixels)
	{
		const float scaled = pixels * Features::menu.getDpiScale();
		return addSpacingUnscaled(scaled);
	}

	void addSpacingUnscaled(const float pixels)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, pixels });
		ImGui::Spacing();
		ImGui::PopStyleVar();
	}

}