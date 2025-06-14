#include "../GameHook.hpp"

#include "../../../SDK/GameClass/UserCmd.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Utils/Logging.hpp"

#include "../../../Features/Visuals/GrenadePrediction.hpp"

void* Hooks::Game::CreateMove::hook_func(void* csgo_input, int unk, UserCmd* usercmd)
{
	void* ret = RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), csgo_input, unk, usercmd);

#ifdef DEBUG
	Logging::info("UserCmd: {}", usercmd);
	if (UserCmd* cmd = usercmd; cmd)
		if (BaseCmd* base_cmd = cmd->csgo_usercmd.base_cmd; base_cmd)
			if (ViewAngles* view_angles = base_cmd->viewangles; view_angles)
				Logging::info("View angles: {}", view_angles->ang_value);
#endif

	grenade_prediction->calculate_grenade_prediction();

	return ret;
}
