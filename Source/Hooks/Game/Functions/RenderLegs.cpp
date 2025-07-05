#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/Removals.hpp"

void Hooks::Game::RenderLegs::hook_func(void* rdi, void* rsi, void* rdx, void* rcx, void* r8)
{
	if (removals->should_remove_legs())
		return;

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), rdi, rsi, rdx, rcx, r8);
}
