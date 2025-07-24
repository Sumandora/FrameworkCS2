#include "Aimbot.hpp"

#include "../Feature.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "../../SDK/EngineTrace/GameTrace.hpp"
#include "../../SDK/EngineTrace/TraceFilter.hpp"
#include "../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../SDK/Entities/Components/BodyComponent.hpp"
#include "../../SDK/Entities/Components/BodyComponentSkeletonInstance.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/CSPlayerWeaponServices.hpp"
#include "../../SDK/Entities/VData/CSWeaponBaseVData.hpp"
#include "../../SDK/Enums/LifeState.hpp"
#include "../../SDK/GameClass/EngineToClient.hpp"
#include "../../SDK/GameClass/SkeletonInstance.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"

#include "../../Utils/BulletSimulation.hpp"
#include "../../Utils/Logging.hpp"
#include "../../Utils/Prediction.hpp"

#include "glm/common.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

#include <cfloat>
#include <cmath>

Aimbot::Aimbot()
	: Feature("Combat", "Aimbot")
{
}

// https://github.com/SwagSoftware/Kisak-Strike/blob/7df358a4599ba02a4e072f8167a65007c9d8d8c/mathlib/mathlib_base.cpp#L1108
static void vector_angles(const glm::vec3& forward, glm::vec3& angles)
{
	const float yaw = glm::degrees(std::atan2(forward.y, forward.x));
	const float tmp = std::sqrt(forward.x * forward.x + forward.y * forward.y);
	const float pitch = glm::degrees(std::atan2(-forward.z, tmp));

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void Aimbot::create_move(UserCmd* cmd)
{
	if (!enabled.get() || !Memory::local_player || !cmd->csgo_usercmd.has_base())
		return;

	// if (!(cmd->buttons.buttonstate1 & IN_ATTACK))
	// 	return;

	// cmd->set_buttonstate1(cmd->buttons.buttonstate1 & ~IN_ATTACK);

	if (!Memory::local_player)
		return;
	if (!Memory::local_player->gameSceneNode())
		return;

	const CSPlayerWeaponServices* weapon_services = Memory::local_player->weapon_services();

	if (!weapon_services)
		return;

	BasePlayerWeapon* weapon = weapon_services->active_weapon().get();

	if (!weapon)
		return;

	const auto* vdata = static_cast<CSWeaponBaseVData*>(weapon->get_vdata());

	if (!vdata->is_gun())
		return;

	if (!Memory::local_player->can_perform_primary_attack())
		return;

	const bool predicted = Prediction::begin(cmd);

	const glm::vec3 from = Interfaces::engine->get_network_client_info().local_data->eye_pos;

	const glm::vec2 rotations{
		cmd->csgo_usercmd.base().viewangles().x(),
		cmd->csgo_usercmd.base().viewangles().y(),
	};

	glm::vec2 rots;
	float fov = FLT_MAX;

	auto* entity_list = GameEntitySystem::the();

	const int highest = entity_list->getHighestEntityIndex();

	for (int i = 0; i < highest; i++) { // TODO only players? can I limit to 64?
		BaseEntity* ent = entity_list->getBaseEntity(i);
		if (!ent)
			continue;

		auto* cs_pawn = ent->entity_cast<CSPlayerPawn*>();
		if (!cs_pawn)
			continue;

		if (cs_pawn->health() <= 0 || cs_pawn->life_state() != LIFE_ALIVE)
			continue;

		if (cs_pawn->team_id() == Memory::local_player->team_id())
			continue;

		BodyComponent* body_component = ent->body_component();
		if (!body_component)
			continue;

		const SkeletonInstance& skeleton = static_cast<BodyComponentSkeletonInstance*>(body_component)->skeleton_instance();
		if (skeleton.bone_count == 0)
			continue;

		if (!skeleton.bones)
			continue;

		const glm::vec3 to = skeleton.bones[BONE_HEAD].pos;

		const BulletSimulation::Results results = BulletSimulation::simulate_bullet(from, to, cs_pawn);
		if (results.scaled_damage < min_damage.get())
			continue;

		const glm::vec3 delta = to - from;

		glm::vec3 needed_rots;
		vector_angles(delta, needed_rots);

		glm::vec2 needed_rotations = needed_rots.xy();
		needed_rotations.x = glm::clamp(needed_rotations.x, -89.0F, 89.0F); // TODO read convars

		needed_rotations -= Memory::local_player->get_aim_punch(); // TODO Use tick fraction...

		glm::vec2 rot_diff = needed_rotations - rotations;
		rot_diff.y = std::remainder(rot_diff.y, 360.0F);

		const float this_fov = glm::length(rot_diff);
		if (this_fov < fov) {
			fov = this_fov;
			rots = needed_rotations;
		}
	}

	if (fov != FLT_MAX) {
		CMsgQAngle* base_viewangles = cmd->csgo_usercmd.mutable_base()->mutable_viewangles();
		base_viewangles->set_x(rots.x);
		base_viewangles->set_y(rots.y);

		// TODO for silent aim, one must insert a single input history if there is none, then just remove the base view angles assignment.

		for (auto& input : *cmd->csgo_usercmd.mutable_input_history()) {
			CMsgQAngle* qangle = input.mutable_view_angles();
			qangle->set_x(rots.x);
			qangle->set_y(rots.y);
		}

		cmd->set_buttonstate2(cmd->buttons.buttonstate2 | IN_ATTACK);

		CSubtickMoveStep* new_step = cmd->allocate_new_move_step(0.0001F);

		new_step->set_button(IN_ATTACK);
		new_step->set_pressed(true);

		cmd->csgo_usercmd.set_attack1_start_history_index(0);

		new_step = cmd->allocate_new_move_step(0.0001F);

		new_step->set_button(IN_ATTACK);
		new_step->set_pressed(false);

		// This means released, technically true since both happened this tick...
		cmd->csgo_usercmd.set_attack3_start_history_index(0);
	}

	if (predicted)
		Prediction::end();
}

bool Aimbot::wants_silent_aim() const
{
	return enabled.get();
}
