#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include <unordered_map>

#include "../../../SDK/GameClass/GameEvent.hpp"

#include "../../../Features/Misc/Hitmarker.hpp"
#include "../../../Features/Misc/VoteRevealer/VoteRevealer.hpp"
#include "../../../Features/PlayerList/PlayerList.hpp"
#include "../../../Features/Visuals/BulletTracers.hpp"
#include "../../../Features/Visuals/GrenadeHelper.hpp"
#include "../../../Features/Visuals/WorldColors.hpp"

void* Hooks::Game::FireEvent::hookFunc(void* game_event_manager, GameEvent* event, bool rdx, bool rcx)
{
	event_counters[event->GetName()]++;

	grenade_helper->event_handler(event);
	hit_marker->event_handler(event);
	vote_revealer->event_handler(event);
	bullet_tracers->event_handler(event);
	player_list->event_handler(event);
	world_colors->event_handler(event);

	return RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), game_event_manager, event, rdx, rcx);
}
