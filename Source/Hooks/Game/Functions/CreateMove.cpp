#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

void* Hooks::Game::CreateMove::hook_func(void* csgo_input, int unk, void* usercmd)
{
	return RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), csgo_input, unk, usercmd);
}
