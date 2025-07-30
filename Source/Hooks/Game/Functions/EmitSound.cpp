#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Utils/Prediction.hpp"

#include <cstdint>

void* Hooks::Game::EmitSound::hook_func(void* return_value, void* recipient_filter, int entity_index, void* emitsound)
{
	// if (Interfaces::client_prediction->in_prediction()) // TODO Check first predict
	if (Prediction::in_prediction()) {
		// I'm just reusing the code inside the game, which early cancels if a global isnt set
		struct EmitSoundRetVal {
			std::uint64_t a;
			std::uint64_t b;
			std::uint64_t c;
		};

		auto* r = reinterpret_cast<EmitSoundRetVal*>(return_value);

		r->a = 0xffffffff00000000;
		r->b = 0;
		r->c = 0;

		return return_value;
	}
	return RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), return_value, recipient_filter, entity_index, emitsound);
}
