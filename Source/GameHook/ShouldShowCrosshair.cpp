#include "GameHook.hpp"

#include <limits>

#include "../SDK/Entities/BaseEntity.hpp"
#include "../SDK/Entities/CSPlayerPawnBase.hpp"

bool forceCrosshair = false;

bool GameHook::ShouldShowCrosshair::hookFunc([[maybe_unused]] void* weapon)
{
	if(forceCrosshair) {
		auto player = BaseEntity::getLocalPlayer();
		if(player == nullptr || !player->isScoped())
			return true;
	}
	return RetAddrSpoofer::invoke<bool>(hook->proxy, weapon);
}
