#include "Prediction.hpp"

#include "../SDK/Entities/CSPlayerController.hpp"
#include "../SDK/Entities/CSPlayerPawn.hpp"
#include "../SDK/Entities/Services/PlayerMovementServices.hpp"
#include "../SDK/GameClass/NetworkClientService.hpp"
#include "../SDK/GameClass/NetworkGameClient.hpp"
#include "../SDK/GameClass/UserCmd.hpp"

#include "../Interfaces.hpp"
#include "../Memory.hpp"

static bool prev_has_been_predicted{};
static UserCmd* prev_cmd{};

static UserCmd* prediction_cmd{};

static bool in_prediction = false;

bool Prediction::begin(UserCmd* cmd)
{
	if (!Memory::local_player)
		return false;

	if (!Memory::globals || !*Memory::globals)
		return false;

	if (!Memory::local_player_controller || !*Memory::local_player_controller)
		return false;

	if (!Interfaces::network_client_service)
		return false;

	NetworkGameClient* network_game_client = Interfaces::network_client_service->network_game_client;

	if (!network_game_client)
		return false;

	PlayerMovementServices* movement_services = Memory::local_player->movement_services();
	if (!movement_services)
		return false;

	prev_has_been_predicted = cmd->has_been_predicted;
	prev_cmd = *(*Memory::local_player_controller)->get_current_command();

	prediction_cmd = cmd;

	::in_prediction = true;

	movement_services->set_prediction_command(cmd);
	*(*Memory::local_player_controller)->get_current_command() = cmd;
	network_game_client->client_side_predict(PredictionStage::CLIENT_COMMAND_TICK);

	return true;
}

void Prediction::end()
{
	// Since in the header, we declared that end may not be called when begin returned false, we can assume that all the variables are non-null.

	Memory::local_player->movement_services()->reset_prediction_command();
	*(*Memory::local_player_controller)->get_current_command() = prev_cmd;

	::in_prediction = false;

	prediction_cmd->has_been_predicted = prev_has_been_predicted;
}

bool Prediction::in_prediction()
{
	return ::in_prediction;
}
