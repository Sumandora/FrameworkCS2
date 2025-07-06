#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/Removals.hpp"

void Hooks::Game::AddSleeveModel::hook_func(void* rdi, void* rsi)
{
	if (removals->should_remove_sleeves())
		return;

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), rdi, rsi);
}
