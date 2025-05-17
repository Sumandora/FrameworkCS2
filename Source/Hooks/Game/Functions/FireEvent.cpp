#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include <unordered_map>

#include "../../../SDK/GameClass/GameEvent.hpp"

std::unordered_map<std::string, std::size_t> eventCounters;

void* Hooks::Game::FireEvent::hookFunc(void* gameEventManager, GameEvent* event, bool rdx, bool rcx) {
	eventCounters[event->GetName()]++;
	return RetAddrSpoofer::invoke<void*>(hook->get_proxy(), gameEventManager, event, rdx, rcx);
}
