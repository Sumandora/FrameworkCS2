#include "CSPlayerPawn.hpp"

#include "Services/CSPlayerItemServices.hpp"

#include "../EngineTrace/GameTrace.hpp"

bool CSPlayerPawn::is_armored_at(HitGroup hit_group) const
{
	if (armor_value() <= 0)
		return false;

	switch (hit_group) {
	case HitGroup::GENERIC:
	case HitGroup::CHEST:
	case HitGroup::STOMACH:
	case HitGroup::LEFTARM:
	case HitGroup::RIGHTARM:
		return true;
		break;
	case HitGroup::HEAD:
		if (auto* item_services = this->item_services();
			item_services && static_cast<CSPlayerItemServices*>(item_services)->has_helmet())
			return true;
		break;
	default:
		break;
	}

	return false;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool CSPlayerPawn::has_heavy_armor() const
{
	// NOTE: Heavy armor was removed from the game by this update:
	// 	https://github.com/SteamDatabase/GameTracking-CS2/commit/a77801f5a6ba82048d235f20e49af027268aabc8
	// 	This was the first out of 2 updates released on July 2nd 2025.
	// I keep this function here, and return a constant value for compatibility reasons and because I believe they might bring it back.
	return false;
}
