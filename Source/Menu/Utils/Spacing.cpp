#include "Spacing.hpp"

#include "../../Config/Config.hpp"

#include <imgui.h>

namespace Menu {
	void addSpacing(const int pixels)
	{
		const float scaled = static_cast<float>(pixels) * Config::getDpiScale();
		return addSpacingUnscaled(scaled);
	}

	void addSpacingUnscaled(const int pixels)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, static_cast<float>(pixels) });
		ImGui::Spacing();
		ImGui::PopStyleVar();
	}

}