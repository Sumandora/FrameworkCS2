#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../SDK/Padding.hpp"

#include "../../../Features/Visuals/Removals.hpp"

struct HudOverlayData {
	PADDING(0x4);
	bool draw_scope;
};

void* Hooks::Game::DrawHudOverlay::hook_func(void* rdi, void* rsi, void* rdx, void* rcx, HudOverlayData* r8)
{
	if (removals->should_remove_scope()) {
		r8->draw_scope = false;
	}

	return RetAddrSpoofer::invoke<void*>(
		reinterpret_cast<void*>(hook->get_trampoline()),
		rdi, rsi, rdx, rcx, r8);
}
