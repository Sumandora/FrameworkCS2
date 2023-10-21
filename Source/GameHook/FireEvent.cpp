#include "GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include <unordered_map>

std::unordered_map<std::string, std::size_t> eventCounters;

void* GameHook::FireEvent::hookFunc(void* gameEventManager, GameEvent* event, bool rdx, bool rcx) {
	eventCounters[event->GetName()]++;
	return RetAddrSpoofer::invoke<void*>(hook->proxy, gameEventManager, event, rdx, rcx);
}
