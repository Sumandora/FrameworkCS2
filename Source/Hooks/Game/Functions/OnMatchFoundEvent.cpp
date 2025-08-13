#include "../GameHook.hpp"

#include "../../../Features/Misc/AutoAccept.hpp"

#include "RetAddrSpoofer.hpp"

#include <string_view>

static constexpr std::string_view MATCH_FOUND_EVENT_NAME = "popup_accept_match_found";

void* Hooks::Game::OnMatchFoundEvent::hook_func(void* rdi, const char* rsi, void* rdx, void* rcx)
{
	void* ret = RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), rdi, rsi, rdx, rcx);

	if(rsi == MATCH_FOUND_EVENT_NAME) {
		auto_accept->on_match_found();
	}

	return ret;
}
