#include "Bhop.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"

#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"

#include "google/protobuf/arena.h"

#include "RetAddrSpoofer.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdint>

Bhop::Bhop()
	: Feature("Misc", "Bhop")
{
}

static bool in_jump = false;

void Bhop::create_move(UserCmd* cmd)
{
	if (!enabled.get() || !Memory::local_player)
		return;

	static auto* allocate_subtick_move
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 49 8D 7D ? 48 89 C6 E8 ? ? ? ? 66 0F EF C9">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<CSubtickMoveStep* (*)(google::protobuf::Arena * arena)>("Couldn't find allocate subtick move");

	const bool holding_jump = cmd->get_buttonstate1() & IN_JUMP;

	cmd->set_buttonstate1(cmd->get_buttonstate1() & ~IN_JUMP);
	if (in_jump) {
		cmd->set_buttonstate2(cmd->get_buttonstate2() | IN_JUMP);

		if (cmd->csgo_usercmd.has_base()) {
			auto* subtick_moves = cmd->csgo_usercmd.mutable_base()->mutable_subtick_moves();

			CSubtickMoveStep* new_step = RetAddrSpoofer::invoke(allocate_subtick_move, subtick_moves->GetArena());

			new_step->set_button(IN_JUMP);
			new_step->set_pressed(false);
			new_step->set_when(0.999F);

			subtick_moves->AddAllocated(new_step);
		}

		in_jump = false;
	} else
		cmd->set_buttonstate2(cmd->get_buttonstate2() & ~IN_JUMP);
	cmd->set_buttonstate3(cmd->get_buttonstate3() & ~IN_JUMP);

	if (!cmd->csgo_usercmd.has_base())
		return;

	auto* subtick_moves = cmd->csgo_usercmd.mutable_base()->mutable_subtick_moves();

	// NOLINTNEXTLINE(modernize-use-ranges)
	auto it = std::remove_if(
		subtick_moves->begin(),
		subtick_moves->end(),
		[](const CSubtickMoveStep& step) { return step.button() == IN_JUMP; });

	if (it != subtick_moves->end())
		subtick_moves->erase(it);

	static constexpr std::uint32_t FL_ONGROUND = 1 << 0;
	if (holding_jump && Memory::local_player->flags() & FL_ONGROUND) {
		cmd->set_buttonstate1(cmd->get_buttonstate1() | IN_JUMP);
		cmd->set_buttonstate2(cmd->get_buttonstate2() | IN_JUMP);
		in_jump = true;

		for (int i = 0; i < 12; i++) {
			CSubtickMoveStep* new_step = RetAddrSpoofer::invoke(allocate_subtick_move, subtick_moves->GetArena());

			new_step->set_button(IN_JUMP);
			new_step->set_pressed(true);
			new_step->set_when(static_cast<float>(i) / static_cast<float>(12));

			subtick_moves->AddAllocated(new_step);
		}
	}
}
