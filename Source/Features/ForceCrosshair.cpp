#include "Features.hpp"

#include "../SDK/Entities/CSPlayerPawnBase.hpp"

bool Features::ForceCrosshair::shouldForce()
{
	if (!Features::ForceCrosshair::enabled)
		return false;

	auto player = BaseEntity::getLocalPlayer();
	return player == nullptr || !player->isScoped();
}