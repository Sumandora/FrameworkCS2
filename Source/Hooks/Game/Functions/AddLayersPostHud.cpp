#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/Removals.hpp"

// TODO This hook annoys me because I remove the entire "PostHud" render pipeline,
// which right now only contains the flash effect from what I can tell, but in the future it might include more.
// Right now I would remove these new things as well, I might want to change this in the future...
void Hooks::Game::AddLayersPostHud::hook_func(
	void* rdi, void* rsi, void* rdx, void* rcx, void* r8, void* r9)
{
	if (removals->should_remove_flash_overlay())
		return;

	RetAddrSpoofer::invoke<void>(
		reinterpret_cast<void*>(hook->get_trampoline()), rdi, rsi, rdx, rcx, r8, r9);
}
