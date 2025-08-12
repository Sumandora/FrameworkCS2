#include "Bhop.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"

#include "../../Utils/Prediction.hpp"

#include "../../Memory.hpp"

#include <cfloat>
#include <cstring>

Bhop::Bhop()
	: Feature("Misc", "Bhop")
{
}

void Bhop::create_move(UserCmd* cmd)
{
	if (!enabled.get() || !Memory::local_player)
		return;

	if (Memory::local_player->move_type() != MOVETYPE_WALK)
		return;

	const bool holding_jump = cmd->buttons.buttonstate1 & IN_JUMP;

	if (!holding_jump)
		return;

	const bool predicted = Prediction::begin(cmd);

	cmd->buttons.buttonstate1 &= ~IN_JUMP;
	cmd->buttons.buttonstate2 &= ~IN_JUMP;

	if (Memory::local_player->flags() & FL_ONGROUND) {
		// TODO like all the timings are wrong here...
		// I should probably add like a "Humanized inputs" option
		cmd->buttons.buttonstate1 |= IN_JUMP;
		cmd->buttons.buttonstate2 |= IN_JUMP;

		CSubtickMoveStep* new_step = cmd->allocate_new_move_step(1.0F);

		new_step->set_button(IN_JUMP);
		new_step->set_pressed(true);

		new_step = cmd->allocate_new_move_step(1.0F);

		new_step->set_button(IN_JUMP);
		new_step->set_pressed(false);
	}

	if (predicted)
		Prediction::end();
}
