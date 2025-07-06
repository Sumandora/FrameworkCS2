#include "Bhop.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/CSPlayerController.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/PlayerMovementServices.hpp"
#include "../../SDK/GameClass/GlobalVars.hpp"
#include "../../SDK/GameClass/NetworkClientService.hpp"
#include "../../SDK/GameClass/NetworkGameClient.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"

#include "../../Memory.hpp"

#include "../../Interfaces.hpp"

#include <cfloat>
#include <cstdint>
#include <cstring>

Bhop::Bhop()
	: Feature("Misc", "Bhop")
{
}

void Bhop::create_move(UserCmd* cmd)
{
	if (!enabled.get() || !Memory::local_player)
		return;

	const GlobalVars prev_globals = **Memory::globals;

	auto flags = Memory::local_player->flags();
	auto tick_base = (*Memory::local_player_controller)->tick_base();
	Memory::local_player->movement_services()->set_prediction_command(cmd);
	*(*Memory::local_player_controller)->get_current_command() = cmd;
	if (auto* network_game_client = Interfaces::network_client_service->network_game_client; network_game_client) {
		network_game_client->client_side_predict(PredictionStage::CLIENT_COMMAND_TICK);
	}

	const bool holding_jump = cmd->buttons.buttonstate1 & IN_JUMP;

	static constexpr std::uint32_t FL_ONGROUND = 1 << 0;
	if (holding_jump && Memory::local_player->flags() & FL_ONGROUND) {
		cmd->buttons.buttonstate1 &= ~IN_JUMP;

		CSubtickMoveStep* new_step = cmd->allocate_new_move_step(0.0001F);

		new_step->set_button(IN_JUMP);
		new_step->set_pressed(true);

		new_step = cmd->allocate_new_move_step(0.0001F);

		new_step->set_button(IN_JUMP);
		new_step->set_pressed(false);
	}

	Memory::local_player->movement_services()->reset_prediction_command();
	*(*Memory::local_player_controller)->get_current_command() = nullptr;
	Memory::local_player->flags() = flags;
	(*Memory::local_player_controller)->tick_base() = tick_base;
	**Memory::globals = prev_globals;
}
