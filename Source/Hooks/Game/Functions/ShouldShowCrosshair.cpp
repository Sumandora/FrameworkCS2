#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/ForceCrosshair.hpp"

bool Hooks::Game::ShouldShowCrosshair::hookFunc([[maybe_unused]] void* weapon)
{
	if (force_crosshair->should_force())
		return true;
	return RetAddrSpoofer::invoke<bool>(reinterpret_cast<void*>(hook->get_trampoline()), weapon);
}
