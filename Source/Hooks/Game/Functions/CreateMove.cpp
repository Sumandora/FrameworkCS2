#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Misc/AutoStrafer.hpp"
#include "../../../Features/Misc/Bhop.hpp"

#include "../../../Memory.hpp"

#include "../../../Utils/CRC.hpp"
#include "../../../Utils/EmergencyCrash.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp" // IWYU pragma: keep
#include "../../../SDK/GameClass/UserCmd.hpp"

#include "../../../Features/Visuals/GrenadeHelper.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cassert>
#include <vector>

void* Hooks::Game::CreateMove::hook_func(void* csgo_input, int esi, char dl)
{
	void* ret = RetAddrSpoofer::invoke<void*>(reinterpret_cast<void*>(hook->get_trampoline()), csgo_input, esi, dl);

	if (!Memory::local_player_controller)
		return ret;

	UserCmd* usercmd = UserCmd::get_current_command(*Memory::local_player_controller);

	if (!usercmd)
		return ret;

#ifndef DISABLE_CRC_VALIDATE
	if (usercmd->csgo_usercmd.has_base() && usercmd->csgo_usercmd.base().has_move_crc()) {
		const std::vector<char> pre = CRC::extract_crc(usercmd);
		[[maybe_unused]] const void* pre_ptr = *reinterpret_cast<void* const*>(&usercmd->csgo_usercmd.base().move_crc());
		emergency_crash_if(!CRC::update_crc(usercmd), "Couldn't update CRC! (pre-cm)");
		const std::vector<char> post = CRC::extract_crc(usercmd);
		[[maybe_unused]] const void* post_ptr = *reinterpret_cast<void* const*>(&usercmd->csgo_usercmd.base().move_crc());

		// The data needs to match, but the pointers need to differ.
		// This way if the computed data mismatches or it doesn't update the CRC at all it fails.
		emergency_crash_if(pre != post, "UserCmd CRC was incorrectly recomputed!");

		// TODO This check does not work, I think the arena tries to reuse memory or something, fix this some time soon.
#if 0
		// I think one could change a button and see if it results in a different CRC, but that seems a bit obnoxious for a simple self test.
		// Can we perhaps store sample CRCs and verify that they come out the right way?
		emergency_crash_if(pre_ptr == post_ptr, "UserCmd CRC did not update!");
#endif
	}
#endif

	if (usercmd->csgo_usercmd.has_base() && usercmd->csgo_usercmd.base().has_viewangles()) {
		const CMsgQAngle& viewangles = usercmd->csgo_usercmd.base().viewangles();
		const glm::vec3 vec = glm::vec3(viewangles.x(), viewangles.y(), viewangles.z());
		grenade_helper->update_viewangles(vec);
	}

	bhop->create_move(usercmd);
	auto_strafer->create_move(usercmd);

	// Update the CRC stored in the UserCmd to accommodate our changes.
	if (usercmd->csgo_usercmd.has_base() && usercmd->csgo_usercmd.base().has_move_crc()) {
		emergency_crash_if(!CRC::update_crc(usercmd), "Couldn't update CRC (post-cm)!");
	}

	return ret;
}
