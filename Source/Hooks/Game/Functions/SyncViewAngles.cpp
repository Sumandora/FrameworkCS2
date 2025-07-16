#include "../GameHook.hpp"

#include "../../../Features/Combat/Aimbot.hpp"

#include "../../../SDK/GameClass/CSGOInput.hpp"

#include "RetAddrSpoofer.hpp"

#include "glm/ext/vector_float3.hpp"

void Hooks::Game::SyncViewAngles::hook_func(CSGOInput* thisptr, int rsi)
{
	if (!aimbot->wants_silent_aim()) {
		RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, rsi);
		return;
	}
	const glm::vec3 original_viewangles = thisptr->view_angles();
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, rsi);
	thisptr->view_angles() = original_viewangles;
}
