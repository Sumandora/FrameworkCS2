#include "../GameHook.hpp"

#include "../../../Features/Visuals/Radar.hpp"
#include "RetAddrSpoofer.hpp"

void Hooks::Game::RadarUpdate::hook_func(GameRadar* thisptr)
{
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr);
	radar->on_radar_update(thisptr);
}
