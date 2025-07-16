#include "../GameHook.hpp"

#include "../../../Utils/Prediction.hpp"
#include "RetAddrSpoofer.hpp"
#include <cstdint>

std::uint64_t Hooks::Game::EmitSound::hook_func(void* recipient_filter, int entity_index, void* emitsound)
{
	if (Prediction::in_prediction())
		// This is the same return value that the if uses, which the hook skips over.
		// I'm just reusing this one, because code must expect this value and thus can't crash or anything.
		return 0xffffffff00000000;
	return RetAddrSpoofer::invoke<std::uint64_t>(reinterpret_cast<void*>(hook->get_trampoline()), recipient_filter, entity_index, emitsound);
}
