#include "AutoStrafer.hpp"

#include "../Feature.hpp"

#include "../../SDK/ConVar/ConVar.hpp"
#include "../../SDK/ConVar/EngineCvar.hpp"
#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"

#include "../../Memory.hpp"

#include "../../Interfaces.hpp"

#include "glm/common.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

#include <cfloat>
#include <cmath>
#include <cstring>
#include <numbers>
#include <optional>

static ConVar* sv_air_max_wishspeed;
static ConVar* sv_airaccelerate;
static ConVar* sv_maxspeed;

AutoStrafer::AutoStrafer()
	: Feature("Misc", "Auto strafer")
{
	allow_hard_turns.add_visible_condition([this] { return mode.get_raw() == AutoStraferMode::Directional; });
	hard_turn_threshold.add_visible_condition([this] { return allow_hard_turns.is_visible() && allow_hard_turns.get_raw(); });
	subtick.add_visible_condition([this] { return mode.get_raw() == AutoStraferMode::Directional; });

	only_when_idle.add_visible_condition([this] { return mode.get_raw() == AutoStraferMode::Forward; });

	sv_air_max_wishspeed = Interfaces::engineCvar->findByName("sv_air_max_wishspeed");
	sv_airaccelerate = Interfaces::engineCvar->findByName("sv_airaccelerate");
	sv_maxspeed = Interfaces::engineCvar->findByName("sv_maxspeed");
}

void AutoStrafer::create_move(UserCmd* cmd)
{
	if (!enabled.get())
		return;

	CSPlayerPawn* local_player = Memory::local_player;
	if (!local_player || !local_player->alive())
		return;

	static std::optional<float> last_wish_direction = 0.0F;

	const float forward = cmd->csgo_usercmd.base().forwardmove();
	const float left = cmd->csgo_usercmd.base().leftmove();

	if (local_player->flags() & FL_ONGROUND && !(cmd->buttons.buttonstate1 & IN_JUMP)) {
		// Only abort if we are not going to be in air again (if bhopping don't abort)
		if (forward == 0.0F && left == 0.0F)
			last_wish_direction.reset(); // We have no direction to move to.
		else
			last_wish_direction = glm::atan(left, forward);
		return;
	}

	switch (mode.get()) {
	case AutoStraferMode::Directional: {
		const float yaw = glm::radians(cmd->csgo_usercmd.base().viewangles().y());

		float wish_direction = 0.0F;
		if (forward != 0.0F || left != 0.0F)
			wish_direction = glm::atan(left, forward);
		else if (last_wish_direction.has_value())
			wish_direction = last_wish_direction.value(); // If we release all keys go to the last known direction
		else
			return;
		last_wish_direction = wish_direction;

		glm::vec3 velocity = local_player->velocity();

		if (glm::length(velocity.xy()) < 1.0F)
			return;

		// TODO eventually the subtick movement prediction should get good enough, so this doesn't need to be locked in place
		const float real_direction = glm::atan(velocity.y, velocity.x) - yaw;

		bool dir = false;

		const auto compute_move = [this, wish_direction, &dir, real_direction](const glm::vec3& velocity, std::optional<bool> force_flip = std::nullopt) {
			const float speed = glm::length(velocity.xy());

			// https://github.com/degeneratehyperbola/NEPS/blob/7e41ae811cf4ae136203093a059e137c8a10796f/NEPS/Hacks/Misc.cpp#L845-L857
			const float term = sv_air_max_wishspeed->value.floatingPoint / sv_airaccelerate->value.floatingPoint / sv_maxspeed->value.floatingPoint * 100.0F / speed;
			const float perfect_delta = acos(term);

			const float delta = std::remainder(wish_direction - real_direction, 2.0F * std::numbers::pi_v<float>);

			if (!force_flip.has_value()) {
				force_flip = delta > 0.0;
			}

			dir = force_flip.value();

			float new_direction = real_direction + (force_flip.value() ? perfect_delta : -perfect_delta);
			if (allow_hard_turns.get() && std::abs(delta) >= glm::radians(hard_turn_threshold.get()))
				new_direction = real_direction + delta; // User wants to make a hard turn, don't smooth it, he might jump into the line of sight of an enemy

			return glm::vec2{
				cos(new_direction),
				sin(new_direction),
			};
		};

		const glm::vec2 main = compute_move(velocity);
		cmd->csgo_usercmd.mutable_base()->set_forwardmove(main.x);
		cmd->csgo_usercmd.mutable_base()->set_leftmove(main.y);

		if (subtick.get()) {
			if (local_player->flags() & FL_ONGROUND) // TODO, why???
				return;
			cmd->csgo_usercmd.mutable_base()->set_forwardmove(0.0F);
			cmd->csgo_usercmd.mutable_base()->set_leftmove(0.0F);

			glm::vec2 prev_move{ 0.0F, 0.0F };

			const int additional_steps = MAX_SUBTICKS_PER_MOVE - cmd->csgo_usercmd.base().subtick_moves_size(); // TODO configurable count

			for (int i = 0; i < additional_steps; i++) {
				CSubtickMoveStep* subtick = cmd->allocate_new_move_step(static_cast<float>(i) / static_cast<float>(additional_steps));
				subtick->set_button(0);
			}

			for (int i = 0; i < cmd->csgo_usercmd.base().subtick_moves_size(); i++) {
				CSubtickMoveStep* subtick = cmd->csgo_usercmd.mutable_base()->mutable_subtick_moves(i);

				velocity += velocity * (1.0F / 64.0F); // TODO very much incorrect

				glm::vec2 should = compute_move(velocity, dir);

				dir = !dir;

				// WTF???
				should = -should;

				subtick->set_analog_forward_delta(should.x - prev_move.x);
				subtick->set_analog_left_delta(should.y - prev_move.y);

				prev_move = should;
			}
		}

		break;
	}
	case AutoStraferMode::Forward: {
		const int change = cmd->csgo_usercmd.base().mousedx();
		if (!only_when_idle.get() || (cmd->csgo_usercmd.base().leftmove() == 0.0F && cmd->csgo_usercmd.base().forwardmove() == 0.0F)) {
			cmd->csgo_usercmd.mutable_base()->set_forwardmove(0.0F);
			cmd->csgo_usercmd.mutable_base()->set_leftmove(-glm::sign(static_cast<float>(change)));
		}
		break;
	}
	}
}
