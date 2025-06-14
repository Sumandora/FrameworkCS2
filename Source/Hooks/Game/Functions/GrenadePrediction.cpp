#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

// This function is responsible for the PIP camera, but I hijack it to store the initial
// state since the m_angStashedShootAngles and m_vecStashedGrenadeThrowPosition seem broken elsewhere.
void Hooks::Game::GrenadePrediction::hook_func(void *rdi, double xmm0, float xmm1) {
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), rdi, xmm0, xmm1);
}
