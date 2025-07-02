#include "AutoStrafer.hpp"

#include "../Feature.hpp"

#include "../../SDK/ConVar/ConVar.hpp"
#include "../../SDK/ConVar/EngineCvar.hpp"
#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/PlayerMovementServices.hpp"
#include "../../SDK/GameClass/UserCmd.hpp"

#include "../../Memory.hpp"

#include "../../Interfaces.hpp"

#include "glm/common.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

#include <cfloat>
#include <cmath>
#include <cstring>
#include <numbers>
#include <optional>

AutoStrafer::AutoStrafer()
	: Feature("Misc", "Auto strafer")
{
	allow_hard_turns.add_visible_condition([this] { return mode.get_raw() == AutoStraferMode::Directional; });
	hard_turn_threshold.add_visible_condition([this] { return allow_hard_turns.is_visible() && allow_hard_turns.get_raw(); });

	only_when_idle.add_visible_condition([this] { return mode.get_raw() == AutoStraferMode::Forward; });
}

void AutoStrafer::create_move(UserCmd* cmd)
{
	if (!enabled.get())
		return;

	CSPlayerPawn* local_player = Memory::local_player;
	if (!local_player || !local_player->alive())
		return;

	static std::optional<float> last_wish_direction = 0.0F;

	const float forward = cmd->csgo_usercmd.base().forwardmove() * 450.0F;
	const float left = cmd->csgo_usercmd.base().leftmove() * 450.0F;

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
		const glm::vec3 velocity = local_player->velocity();
		const float speed = glm::length(velocity.xy());

		if (speed < 5.0F)
			return;

		static ConVar* sv_air_max_wishspeed = Interfaces::engineCvar->findByName("sv_air_max_wishspeed");
		static ConVar* sv_airaccelerate = Interfaces::engineCvar->findByName("sv_airaccelerate");
		static ConVar* sv_maxspeed = Interfaces::engineCvar->findByName("sv_maxspeed");

		// https://github.com/degeneratehyperbola/NEPS/blob/7e41ae811cf4ae136203093a059e137c8a10796f/NEPS/Hacks/Misc.cpp#L845-L857
		const float term = sv_air_max_wishspeed->value.floatingPoint / sv_airaccelerate->value.floatingPoint / sv_maxspeed->value.floatingPoint * 100.0F / speed;

		if (term <= -1.0F || term >= 1.0F)
			return;

		const float perfect_delta = acos(term);

		const float yaw = glm::radians(cmd->csgo_usercmd.base().viewangles().y());
		const float real_direction = glm::atan(velocity.y, velocity.x) - yaw;

		float wish_direction = 0.0F;
		if (forward != 0.0F || left != 0.0F)
			wish_direction = glm::atan(left, forward);
		else if (last_wish_direction.has_value())
			wish_direction = last_wish_direction.value(); // If we release all keys go to the last known direction
		else
			return;
		last_wish_direction = wish_direction;

		const float delta = std::remainder(wish_direction - real_direction, 2.0F * std::numbers::pi_v<float>);

		float new_direction = real_direction + (delta > 0.0 ? perfect_delta : -perfect_delta);
		if (allow_hard_turns.get() && std::abs(delta) >= glm::radians(hard_turn_threshold.get()))
			new_direction = real_direction + delta; // User wants to make a hard turn, don't smooth it, he might jump into the line of sight of an enemy

		cmd->csgo_usercmd.mutable_base()->set_forwardmove(cos(new_direction));
		cmd->csgo_usercmd.mutable_base()->set_leftmove(sin(new_direction));
		break;
	}
	case AutoStraferMode::Forward: {
		const int change = cmd->csgo_usercmd.base().mousedx();
		if (!only_when_idle.get() || (cmd->csgo_usercmd.base().leftmove() == 0.0F && cmd->csgo_usercmd.base().forwardmove() == 0.0F)) {
			cmd->csgo_usercmd.mutable_base()->set_forwardmove(0.0F);
			cmd->csgo_usercmd.mutable_base()->set_leftmove(glm::sign(static_cast<float>(change)));
		}
		break;
	}
	}
}
