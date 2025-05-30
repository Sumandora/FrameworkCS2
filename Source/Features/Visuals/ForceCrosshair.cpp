#include "ForceCrosshair.hpp"

#include "../Feature.hpp"

#include "../../Memory.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"

ForceCrosshair::ForceCrosshair()
	: Feature("Visuals", "Force crosshair")
{
}

bool ForceCrosshair::should_force() const
{
	if (!enabled.get())
		return false;

	auto* player = Memory::local_player;
	return player != nullptr && player->health() > 0 && !player->isScoped();
}
