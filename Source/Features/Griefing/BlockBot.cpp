#include "BlockBot.hpp"

#include "../Feature.hpp"

#include "../../SDK/ConVar/ConVar.hpp"
#include "../../SDK/ConVar/EngineCvar.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/GameEntitySystem.hpp"
#include "../../SDK/EntityHandle.hpp"
#include "../../SDK/GameClass/EngineToClient.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"
#include "../../SDK/TickTimeConversion.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "../../Utils/MovementQuantization.hpp"
#include "../../Utils/Projection.hpp"
#include "../../Utils/SmoothedPing.hpp"

#include "glm/common.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

#include "imgui.h"

#include <atomic>
#include <cmath>
#include <ctime>
#include <mutex>
#include <optional>
#include <utility>

static ConVar* sv_subtick_movement_view_angles = nullptr;
static ConVar* sv_friction = nullptr;
static ConVar* sv_stopspeed = nullptr;

BlockBot::BlockBot()
	: Feature("Griefing", "Block bot")
{
	slow_walk_threshold.add_visible_condition([this] {
		return slow_walk_when_close_to_target.get_raw();
	});

	sv_subtick_movement_view_angles = Interfaces::engineCvar->findByName("sv_subtick_movement_view_angles");
	MEM_ACCEPT(sv_subtick_movement_view_angles);
	sv_friction = Interfaces::engineCvar->findByName("sv_friction");
	MEM_ACCEPT(sv_friction);
	sv_stopspeed = Interfaces::engineCvar->findByName("sv_stopspeed");
	MEM_ACCEPT(sv_stopspeed);
}

CSPlayerPawn* BlockBot::find_new_target(GameSceneNode* local_game_scene_node)
{
	float best_distance = 0.0F;
	CSPlayerPawn* best_entity = nullptr;

	const glm::vec3 local_pos = local_game_scene_node->transform().m_Position;

	for (CSPlayerPawn* entity : GameEntitySystem::the()->entities_of_type<CSPlayerPawn>()) {
		if (entity == Memory::local_player)
			continue;

		GameSceneNode* game_scene_node = entity->gameSceneNode();
		if (!game_scene_node)
			continue;

		const glm::vec3 pos = game_scene_node->transform().m_Position;

		const float dist = glm::distance(local_pos, pos);

		if (!best_entity || dist < best_distance) {
			best_distance = dist;
			best_entity = entity;
		}
	}

	return best_entity;
}

glm::vec3 BlockBot::calculate_direction_vector(glm::vec3 local_pos, glm::vec3 other_pos)
{
	const glm::vec3 delta = other_pos - local_pos;
	const glm::vec3 norm = glm::normalize(delta);

	glm::vec3 potentially_rounded;
	switch (mode.get()) {
	case BlockBotMode::Round_to_45:
		potentially_rounded = glm::round(norm);
		break;
	case BlockBotMode::Round_to_90:
		if (std::abs(norm.x) > std::abs(norm.y))
			potentially_rounded = glm::vec3{ norm.x, 0.0F, norm.z };
		else if (std::abs(norm.x) < std::abs(norm.y))
			potentially_rounded = glm::vec3{ 0.0F, norm.y, norm.z };
		else
			potentially_rounded = {};
		break;
	case BlockBotMode::Free:
		potentially_rounded = norm;
		break;
	}

	static constexpr float RAD90 = glm::radians(90.0F);

	const glm::vec3 right{
		potentially_rounded.x * glm::cos(RAD90) + potentially_rounded.y * glm::sin(RAD90),
		-potentially_rounded.x * glm::sin(RAD90) + potentially_rounded.y * glm::cos(RAD90),
		0.0F /* This is purely about horizontal movement. */
	};

	return right;
}

void BlockBot::reset()
{
	last_target = EntityHandle<CSPlayerPawn>::invalid();
	const std::lock_guard lock{ visuals_mutex };
	visuals = {};
}

static glm::vec3 friction(CSPlayerPawn* player, glm::vec3 velocity)
{
	// @see CGameMovement::Friction
	const float speed = glm::length(velocity);

	if (speed < 0.1F)
		return velocity;

	const float friction = sv_friction->get_float() * player->movement_services()->surface_friction();
	const float control = (speed < sv_stopspeed->get_float()) ? sv_stopspeed->get_float() : speed;
	const float drop = control * friction * TICK_INTERVAL;
	const float newspeed = glm::max(speed - drop, 0.0F);

	if (newspeed != speed) {
		velocity *= newspeed / speed;
	}

	return velocity;
}

void BlockBot::create_move(UserCmd* cmd)
{
	if (!enabled.get() || !Memory::local_player) {
		reset();
		return;
	}

	GameSceneNode* local_game_scene_node = Memory::local_player->gameSceneNode();
	if (!local_game_scene_node) {
		reset();
		return;
	}

	const NetworkedClientInfo network_client_info = Interfaces::engine->get_network_client_info();

	if (!network_client_info.local_data)
		return;

	const glm::vec3 local_pos = local_game_scene_node->transform().m_Position;

	CSPlayerPawn* current_target = last_target.get();

	glm::vec3 direction_vector = last_direction_vector;

	if (!current_target) {
		current_target = find_new_target(local_game_scene_node);
		if (current_target) {
			// Found new target
			direction_vector = calculate_direction_vector(local_pos, current_target->gameSceneNode()->transform().m_Position);
			last_target = current_target->get_handle();
		} else {
			reset();
			return;
		}
	}

	last_direction_vector = direction_vector;

	// Compute the position to go in direct opposition:
	const bool add_ping = add_ping_to_extrapolation_ticks.get();
	const float additional_ping_ticks = add_ping ? time_to_ticks_with_partial(average_ping.get_average() / 1000.0F) : 0;
	const float self_interp_ticks = local_extrapolation_ticks.get() + additional_ping_ticks;
	const float target_interp_ticks = target_extrapolation_ticks.get() + additional_ping_ticks;
	const glm::vec3 a = current_target->gameSceneNode()->transform().m_Position + current_target->abs_velocity() * TICK_INTERVAL * target_interp_ticks;
	const glm::vec3 velocity = friction(Memory::local_player, Memory::local_player->abs_velocity()) * TICK_INTERVAL;
	const glm::vec3 b = local_pos + velocity * self_interp_ticks;
	const glm::vec3 delta = b - a;

	const glm::vec3 dot = glm::dot(delta, direction_vector) * direction_vector;
	const glm::vec3 perpendicular = delta - dot;
	const glm::vec3 p = a + perpendicular;

	{
		const std::lock_guard lock{ visuals_mutex };
		visuals = Visuals{
			.from = a,
			.local_pos = b,
			.direction_vector = direction_vector,
			.movement_target = p,
		};
	}

	// Walk to the position
	const glm::vec3 v = p - b;

	float yaw = network_client_info.local_data->viewangles.y;

	CBaseUserCmdPB* base = cmd->csgo_usercmd.mutable_base();

	// Either we do this for a stable move fix, or we do this in order to avoid creating too many
	// subticks with rotations to make our movement changes fail.
	if (MovementQuantization::is_movement_quantized() || sv_subtick_movement_view_angles->get_bool()) {
		const float base_yaw = glm::degrees(glm::atan(v.y, v.x));
		const float delta_yaw = yaw - base_yaw;
		static constexpr float MAX_ANGLE = 360.0F;
		static constexpr float DIRECTIONS_POSSIBLE_WITH_WASD = 8.0F; // since opposing keys cancel each other out
		static constexpr float MIN_ANGLE_INCREASE_WITH_WASD = MAX_ANGLE / DIRECTIONS_POSSIBLE_WITH_WASD;
		const float best = base_yaw + std::round(delta_yaw / MIN_ANGLE_INCREASE_WITH_WASD) * MIN_ANGLE_INCREASE_WITH_WASD;
		const float wrapped = std::remainder(best, 360.0F); // For good measure
		base->mutable_viewangles()->set_y(wrapped);
		yaw = wrapped;
	}

	const float v_length = glm::length(v);
	if (v_length <= 3.0F)
		return; // good enough...

	const float direction = glm::atan(v.y, v.x) - glm::radians(yaw);

	const glm::vec2 movement = glm::normalize(glm::vec2{
		cos(direction),
		sin(direction),
	});

	// Let the user override the movement, perhaps they want to walk closer.
	bool moved = false;

	if (base->forwardmove() == 0) {
		base->set_forwardmove(movement.x);
		moved = true;
	}
	if (base->leftmove() == 0) {
		base->set_leftmove(movement.y);
		moved = true;
	}

	if (moved && slow_walk_when_close_to_target.get() && v_length < slow_walk_threshold.get())
		cmd->buttons.buttonstate1 |= IN_SPEED;
}

void BlockBot::draw(ImDrawList* draw_list)
{
	std::optional<Visuals> v;

	{
		const std::lock_guard lock{ visuals_mutex };
		v = visuals;
	}

	if (!v.has_value())
		return;

	ImVec2 screen;
	if (Projection::project(v->from, screen)) {
		draw_list->AddCircle(screen, 2.5F, ImColor{ 0.0F, 1.0F, 0.0F, 1.0F });
	}

	const glm::vec3 line_a = v->local_pos + v->direction_vector * -100.0F;
	const glm::vec3 line_b = v->local_pos + v->direction_vector * 100.0F;
	ImVec2 from;
	ImVec2 to;
	if (Projection::project(line_a, from) && Projection::project(line_b, to)) {
		draw_list->AddLine(from, to, -1);
	}

	ImVec2 p;
	if (Projection::project(v->movement_target, p)) {
		draw_list->AddCircle(p, 2.5F, ImColor{ 1.0F, 0.0F, 0.0F, 1.0F });
	}
}

bool BlockBot::wants_silent_aim() const
{
	return enabled.get() && MovementQuantization::is_movement_quantized();
}
