#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Features.hpp"

bool Hooks::Game::ShouldShowCrosshair::hookFunc([[maybe_unused]] void* weapon)
{
	if (Features::force_crosshair->should_force())
		return true;
	return RetAddrSpoofer::invoke<bool>(reinterpret_cast<void*>(hook->get_trampoline()), weapon);
}
