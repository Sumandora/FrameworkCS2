#include "../GameHook.hpp"

#include "../../../SDK/GameClass/UserCmd.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Memory.hpp"
#include "../../../Utils/Logging.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "glm/ext/vector_float3.hpp"

void* Hooks::Game::CreateMove::hook_func(void* csgo_input, int unk, UserCmd* usercmd)
{
	void* ret = RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), csgo_input, unk, usercmd);
	if (!Memory::local_player_controller)
		return ret;

	UserCmd* usercmd = UserCmd::get_current_command(*Memory::local_player_controller);

	Logging::info("UserCmd: {}", usercmd);
	if (UserCmd* cmd = usercmd; cmd) {
		if (usercmd->csgo_usercmd.has_base())
			if (usercmd->csgo_usercmd.base().has_viewangles()) {
				const CMsgQAngle& viewangles = usercmd->csgo_usercmd.base().viewangles();
				Logging::info("View angles: {}", glm::vec3(viewangles.x(), viewangles.y(), viewangles.z()));
			}
	}

	return ret;
}
