#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include <unordered_map>

#include "../../../SDK/GameClass/GameEvent.hpp"

void* Hooks::Game::FireEvent::hookFunc(void* gameEventManager, GameEvent* event, bool rdx, bool rcx) {
	event_counters[event->GetName()]++;
	return RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), gameEventManager, event, rdx, rcx);
}
