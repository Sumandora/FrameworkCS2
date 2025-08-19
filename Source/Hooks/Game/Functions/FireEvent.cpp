#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include <string>
#include <string_view>
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
	const std::string_view event_name = event->GetName();

	event_counters[std::string{ event_name }]++;

	grenade_helper->event_handler(event, event_name);
	hit_marker->event_handler(event, event_name);
	vote_revealer->event_handler(event, event_name);
	bullet_tracers->event_handler(event, event_name);
	player_list->event_handler(event, event_name);
	world_colors->event_handler(event_name);

	return RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), game_event_manager, event, rdx, rcx);
}
