#include "../GameHook.hpp"

#include "../../../SDK/Entities/MapInfo.hpp"

#include "../../../Features/Misc/BombTimer.hpp"

#include "RetAddrSpoofer.hpp"

void Hooks::Game::UpdateBombRadius::hook_func(MapInfo* thisptr, const char* key, const char* value)
{
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, key, value);

	// This feels like the most invasive, aggressive way of obtaining this, but I can't find a better way...
	bomb_timer->update_bomb_radius(thisptr->bomb_radius());
}
