#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/LoadingTextChanger.hpp"

const char* Hooks::Game::GetFunLoading::hook_func(const char* fallback)
{
	if (loading_text_changer->is_enabled())
		// TODO I'm reasonably sure this can be a const char*, but I'm not sure if they might write to it...
		return loading_text_changer->get_replacement().c_str();

	return RetAddrSpoofer::invoke<char*>(reinterpret_cast<void*>(hook->get_trampoline()), fallback);
}
