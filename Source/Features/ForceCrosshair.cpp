#include "Features.hpp"

#include "../SDK/Entities/CSPlayerPawn.hpp"

bool Features::ForceCrosshair::shouldForce()
{
	if (!Features::ForceCrosshair::enabled)
		return false;

	auto* player = Memory::local_player;
	return player != nullptr && player->health() > 0 && !player->isScoped();
}

void Features::ForceCrosshair::imguiRender()
{
	if (ImGui::Begin("Force crosshair")) {
		ImGui::Checkbox("Enabled", &enabled);
	}
	ImGui::End();
}
