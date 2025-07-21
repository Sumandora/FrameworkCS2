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

	const bool holding_jump = cmd->buttons.buttonstate1 & IN_JUMP;

	if (!holding_jump)
		return;

	const bool predicted = Prediction::begin(cmd);

	if (Memory::local_player->flags() & FL_ONGROUND) {
		cmd->buttons.buttonstate1 &= ~IN_JUMP;

		CSubtickMoveStep* new_step = cmd->allocate_new_move_step(0.0001F);

		new_step->set_button(IN_JUMP);
		new_step->set_pressed(true);

		new_step = cmd->allocate_new_move_step(0.0001F);

		new_step->set_button(IN_JUMP);
		new_step->set_pressed(false);
	}

	if (predicted)
		Prediction::end();
}
