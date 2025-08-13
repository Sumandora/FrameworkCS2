#include "ForceCrosshair.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/CSPlayerPawn.hpp"

#include "Removals.hpp"

#include "../../Memory.hpp"

ForceCrosshair::ForceCrosshair()
	: Feature("Visuals", "Force crosshair")
{
}

bool ForceCrosshair::should_force() const
{
	if (!enabled.get())
		return false;

	auto* player = Memory::local_player;
	return player != nullptr && player->health() > 0 && (!player->is_scoped() || removals->should_remove_scope());
}
