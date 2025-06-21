#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include <unordered_map>

#include "../../../SDK/GameClass/GameEvent.hpp"

#include "../../../Features/Visuals/GrenadeHelper.hpp"

void* Hooks::Game::FireEvent::hookFunc(void* game_event_manager, GameEvent* event, bool rdx, bool rcx)
{
	event_counters[event->GetName()]++;

	grenade_helper->event_handler(event);

	return RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), game_event_manager, event, rdx, rcx);
}
