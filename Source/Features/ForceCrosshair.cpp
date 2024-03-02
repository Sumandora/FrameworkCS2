#include "Features.hpp"

#include "../SDK/Entities/CSPlayerPawnBase.hpp"
#include "imgui.h"

Features::ForceCrosshair::ForceCrosshair() : Feature("ForceCrosshair") {

}

bool Features::ForceCrosshair::shouldForce()
{
	if (!*Features::ForceCrosshair::enabled)
		return false;

	auto player = BaseEntity::getLocalPlayer();
	return player == nullptr || !player->isScoped();
}

void Features::ForceCrosshair::imguiRender()
{
	if (ImGui::Begin("Force crosshair")) {
		ImGui::Checkbox("Enabled", enabled.getPointer());
	}
	ImGui::End();
}