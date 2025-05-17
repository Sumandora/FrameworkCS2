#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Features.hpp"

bool Hooks::Game::ShouldShowCrosshair::hookFunc([[maybe_unused]] void* weapon)
{
	if (Features::ForceCrosshair::shouldForce())
		return true;
	return RetAddrSpoofer::invoke<bool>(hook->get_proxy(), weapon);
}
