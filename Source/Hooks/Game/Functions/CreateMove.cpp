#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Combat/Aimbot.hpp"
#include "../../../Features/Griefing/BlockBot.hpp"
#include "../../../Features/Misc/AutoStrafer.hpp"
#include "../../../Features/Misc/Bhop.hpp"
#include "../../../Features/Visuals/BulletTracers.hpp"
#include "../../../Features/Visuals/ForceThirdPerson.hpp"
#include "../../../Features/Visuals/GrenadeHelper.hpp"

#include "../../../Memory.hpp"

#include "../../../Utils/CRC.hpp"
#include "../../../Utils/EmergencyCrash.hpp"
#include "../../../Utils/Prediction.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp" // IWYU pragma: keep
#include "../../../SDK/GameClass/CSGOInput.hpp"
#include "../../../SDK/GameClass/UserCmd.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>

void Hooks::Game::CreateMove::hook_func(CSGOInput* csgo_input, int esi, char dl)
{
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), csgo_input, esi, dl);

	if (!Memory::local_player_controller || !*Memory::local_player_controller)
		return;

	UserCmd* usercmd = UserCmd::get_current_command(*Memory::local_player_controller);

	if (!usercmd)
		return;

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
		force_third_person->update_viewangles(vec);
		bullet_tracers->update_viewangles(vec);
	}

	const float forward = usercmd->csgo_usercmd.base().forwardmove();
	const float left = usercmd->csgo_usercmd.base().leftmove();

	const float pitch = usercmd->csgo_usercmd.base().viewangles().x();
	const float yaw = usercmd->csgo_usercmd.base().viewangles().y();

	auto_strafer->create_move(usercmd);
	block_bot->create_move(usercmd);

	bhop->create_move(usercmd);
	aimbot->create_move(usercmd);

	const float new_forward = usercmd->csgo_usercmd.base().forwardmove();
	const float new_left = usercmd->csgo_usercmd.base().leftmove();

	const float new_pitch = usercmd->csgo_usercmd.base().viewangles().x();
	const float new_yaw = usercmd->csgo_usercmd.base().viewangles().y();

	const bool fake_movement = forward != new_forward || left != new_left;
	const bool fake_rotation = yaw != new_yaw || pitch != new_pitch;

	struct LastCmd {
		std::int32_t tick;

		float forwardmove;
		float leftmove;

		float pitch;
		float yaw;

		Buttons buttons;

		bool was_fake_movement;
		bool was_fake_rotation;
	};
	static std::optional<LastCmd> last_cmd = std::nullopt;

	if (last_cmd) {
		if (fake_movement || last_cmd->was_fake_movement) {
			// TODO: This is not accurate, but I think it is still better to do this incorrectly, than not at all.
			//		 Because these states are dependent on subticks, I think they are not verifiable by valve,
			// 		 however I might be wrong because of subtick moves and input history vectors inside the command.
			// 		 To do this very correctly I might have to add my own... I'm not sure if they are checking this already, however.
			//
			// TODO: Changed some things, is it more correct now?
			usercmd->fixup_buttons_for_move(last_cmd->forwardmove, last_cmd->leftmove, last_cmd->buttons);
		}

		if (fake_rotation || last_cmd->was_fake_rotation) {
			// If there is no rotation subtick, then we need to create one.
			if (new_yaw != last_cmd->yaw || new_pitch != last_cmd->pitch)
				usercmd->add_single_rotation_subtick();

			// Not sure if this is good, but I feel like it.
			usercmd->spread_out_rotation_changes(last_cmd->yaw, last_cmd->pitch);
		}
	}

	if (!usercmd->has_been_predicted) {
		// std::cout << usercmd->stringify() << std::endl;
	}

	if (!usercmd->has_been_predicted)
		last_cmd = {
			.tick = usercmd->csgo_usercmd.base().client_tick(),
			.forwardmove = usercmd->csgo_usercmd.base().forwardmove(),
			.leftmove = usercmd->csgo_usercmd.base().leftmove(),
			.pitch = usercmd->csgo_usercmd.base().viewangles().x(),
			.yaw = usercmd->csgo_usercmd.base().viewangles().y(),
			.buttons = usercmd->buttons,
			.was_fake_movement = fake_movement,
			.was_fake_rotation = fake_rotation,
		};

	// Update the CRC stored in the UserCmd to accommodate our changes.
	if (usercmd->csgo_usercmd.has_base() && usercmd->csgo_usercmd.base().has_move_crc()) {
		emergency_crash_if(!CRC::update_crc(usercmd), "Couldn't update CRC (post-cm)!");
	}
}
