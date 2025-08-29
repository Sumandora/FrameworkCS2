#include "TriggerBot.hpp"

#include "../Feature.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/CSPlayerWeaponServices.hpp"
#include "../../SDK/Entities/VData/CSWeaponBaseVData.hpp"
#include "../../SDK/Enums/LifeState.hpp"
#include "../../SDK/GameClass/CSGOInput.hpp"
#include "../../SDK/GameClass/EngineToClient.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"

#include "../../Utils/BulletSimulation.hpp"
#include "../../Utils/Prediction.hpp"

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

#include <cfloat>

TriggerBot::TriggerBot()
	: Feature("Combat", "Trigger bot")
{
}

void TriggerBot::create_move(UserCmd* cmd)
{
	if (!enabled.get() || !Memory::local_player || !cmd->csgo_usercmd.has_base())
		return;

	if (!Memory::local_player)
		return;

	if (!Memory::local_player->gameSceneNode())
		return;

	// TODO this leads to prediction errors.
	// if (!Memory::local_player->can_perform_primary_attack())
	// 	return;

	const CSPlayerWeaponServices* weapon_services = Memory::local_player->weapon_services();

	if (!weapon_services)
		return;

	BasePlayerWeapon* weapon = weapon_services->active_weapon().get();

	if (!weapon)
		return;

	const auto* vdata = static_cast<CSWeaponBaseVData*>(weapon->get_vdata());

	if (!vdata->is_gun())
		return;

	const NetworkedClientInfo network_client_info = Interfaces::engine->get_network_client_info();

	if (network_client_info.local_data == nullptr)
		// TODO
		return;

	const glm::vec3 from = network_client_info.local_data->eye_pos;

	glm::vec2 rotations{
		cmd->csgo_usercmd.base().viewangles().x(),
		cmd->csgo_usercmd.base().viewangles().y(),
	};

	if (cmd->csgo_usercmd.input_history_size() > 0) {
		CSGOInputHistoryEntryPB& input = (*cmd->csgo_usercmd.mutable_input_history())[0];
		rotations.x = input.view_angles().x();
		rotations.y = input.view_angles().y();

		input.set_render_tick_count(network_client_info.render_tick_count);
		input.set_render_tick_fraction(network_client_info.render_tick_fraction);
		input.set_player_tick_count(network_client_info.player_tick_count);
		input.set_player_tick_fraction(network_client_info.player_tick_fraction);
	}

	const class RAIIPrediction {
		bool predicted;

	public:
		explicit RAIIPrediction(UserCmd* cmd)
			: predicted(Prediction::begin(cmd))
		{
		}
		~RAIIPrediction()
		{
			if (predicted)
				Prediction::end();
		}
	} pred{ cmd };

	const float pitch = glm::radians(rotations.x);
	const float yaw = glm::radians(rotations.y);
	const glm::vec3 dir{
		glm::cos(yaw) * glm::cos(pitch),
		glm::sin(yaw) * glm::cos(pitch),
		-glm::sin(pitch),
	};

	const glm::vec3 to = from + dir * vdata->range();

	const BulletSimulation::Results results = BulletSimulation::simulate_bullet(from, to, nullptr);

	// TODO Smoke check.

	const bool should_shoot
		= results.hit_entity
		&& (results.hit_entity->health() > 0 && results.hit_entity->life_state() == LIFE_ALIVE)
		&& (results.hit_entity->team_id() != Memory::local_player->team_id())
		&& (!disable_penetration.get() || results.penetration_count == 0)
		&& results.scaled_damage >= min_damage.get();

	if (should_shoot) {
		cmd->set_buttonstate2(cmd->buttons.buttonstate2 | IN_ATTACK);

		CSubtickMoveStep* new_step = cmd->allocate_new_move_step(1.0F, true);

		new_step->set_button(IN_ATTACK);
		new_step->set_pressed(true);

		cmd->csgo_usercmd.set_attack1_start_history_index(cmd->csgo_usercmd.input_history_size() - 1);

		new_step = cmd->allocate_new_move_step(1.0F, true);

		new_step->set_button(IN_ATTACK);
		new_step->set_pressed(false);

		// This means released, technically true since both happened this tick...
		cmd->csgo_usercmd.set_attack3_start_history_index(cmd->csgo_usercmd.input_history_size() - 1);
	}
}
