#include "GameHook.hpp"

#include <limits>

#include "RetAddrSpoofer.hpp"
#include "../Features/Features.hpp"


bool GameHook::ShouldShowCrosshair::hookFunc([[maybe_unused]] void* weapon)
{
	if(Features::ForceCrosshair::shouldForce())
		return true;
	return RetAddrSpoofer::invoke<bool>(hook->proxy, weapon);
}
