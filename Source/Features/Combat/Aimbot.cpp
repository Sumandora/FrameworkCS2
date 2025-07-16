#include "Aimbot.hpp"

#include "../Feature.hpp"

#include "../../Memory.hpp"

#include "../../SDK/EngineTrace/GameTrace.hpp"
#include "../../SDK/EngineTrace/TraceFilter.hpp"
#include "../../SDK/Entities/Components/BodyComponent.hpp"
#include "../../SDK/Entities/Components/BodyComponentSkeletonInstance.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Enums/LifeState.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"
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
#include "networkbasetypes.pb.h"

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

	auto* entity_list = GameEntitySystem::the();

	const int highest = entity_list->getHighestEntityIndex();

	if (!Memory::local_player)
		return;
	if (!Memory::local_player->gameSceneNode())
		return;

	const bool predicted = Prediction::begin(cmd);

	const glm::vec3 from = Memory::local_player->gameSceneNode()->transform().m_Position + Memory::local_player->view_offset();

	const glm::vec2 rotations{
		cmd->csgo_usercmd.base().viewangles().x(),
		cmd->csgo_usercmd.base().viewangles().y(),
	};

	glm::vec2 rots;
	float fov = FLT_MAX;

	for (int i = 0; i < highest; i++) { // TODO only players? can I limit to 64?
		BaseEntity* ent = entity_list->getBaseEntity(i);
		if (!ent)
			continue;

		auto* cs_pawn = ent->entity_cast<CSPlayerPawn*>();
		if (!cs_pawn)
			continue;

		if (cs_pawn->health() <= 0 || cs_pawn->life_state() != LifeState::ALIVE)
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

		if (Memory::local_player->shots_fired() > 1) {
			// TODO
			// static auto* update_aim_punch_cache
			// 	= BCRL::signature(
			// 		Memory::mem_mgr,
			// 		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 56 41 55 4C 8D 6D ? 41 54 4C 8D 75 ? 49 89 F4">(),
			// 		BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			// 		  .expect<void (*)(CSPlayerPawn*, glm::vec3*, double, float)>("Couldn't find update_aim_punch_cache");
			// glm::vec3 vec;
			// update_aim_punch_cache(Memory::local_player, &vec, 0.0, 0.0F);
			const glm::vec3 aim_punch_angle = Memory::local_player->aim_punch_cache().elements[Memory::local_player->aim_punch_cache().size - 1];
			needed_rotations -= aim_punch_angle.xy() * 2.0F;
			Logging::info("{}", aim_punch_angle.xyz());
		}

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

		new_step = cmd->allocate_new_move_step(0.0001F);

		new_step->set_button(IN_ATTACK);
		new_step->set_pressed(false);
	}

	if (predicted)
		Prediction::end();
}

bool Aimbot::wants_silent_aim() const
{
	return enabled.get();
}
