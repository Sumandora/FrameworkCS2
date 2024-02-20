#include "Features.hpp"

#include "../SDK/Entities/Player/CSPlayerPawnBase.hpp"

bool Features::ForceCrosshair::shouldForce()
{
	if (!Features::ForceCrosshair::enabled)
		return false;

	auto player = BaseEntity::getLocalPlayer();
	return player == nullptr || !player->isScoped();
}

void Features::ForceCrosshair::imguiRender()
{
	if (ImGui::Begin("Force crosshair")) {
		ImGui::Checkbox("Enabled", &enabled);
	}
	ImGui::End();
}