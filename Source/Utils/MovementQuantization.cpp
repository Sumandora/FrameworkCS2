#include "MovementQuantization.hpp"

#include "../SDK/ConVar/ConVar.hpp"
#include "../SDK/ConVar/EngineCvar.hpp"
#include "../SDK/GameClass/UserCmd.hpp"

#include "../Interfaces.hpp"

#include "glm/trigonometric.hpp"

#include "Logging.hpp"

#include <cmath>

static ConVar* sv_quantize_movement_input = nullptr;

void MovementQuantization::init()
{
	sv_quantize_movement_input = Interfaces::engineCvar->findByName("sv_quantize_movement_input");
	Logging::info("sv_quantize_movement_input: {}", sv_quantize_movement_input);
}

bool MovementQuantization::is_movement_quantized()
{
	return sv_quantize_movement_input && sv_quantize_movement_input->get_bool();
}

void MovementQuantization::circumvent_quantization(UserCmd* cmd)
{
	const float yaw = cmd->csgo_usercmd.base().viewangles().y();

	const float pre_forward = cmd->csgo_usercmd.base().forwardmove();
	const float pre_left = cmd->csgo_usercmd.base().leftmove();

	const float old_yaw = glm::degrees(glm::atan(pre_forward, pre_left));

	const float new_forward = std::round(pre_forward);
	const float new_left = std::round(pre_left);

	cmd->csgo_usercmd.mutable_base()->set_forwardmove(new_forward);
	cmd->csgo_usercmd.mutable_base()->set_leftmove(new_left);

	const float new_yaw = glm::degrees(glm::atan(new_forward, new_left));

	// This is the yaw "lost" by the rounding, this means it has to be accounted for in the new yaw
	const float irrecoverable_delta = new_yaw - old_yaw;

	// Wrap to 360 for good measure
	const float fixed_yaw = std::remainder(yaw + irrecoverable_delta, 360.0F);

	cmd->csgo_usercmd.mutable_base()->mutable_viewangles()->set_y(fixed_yaw);

	// TODO subticks (Should they be "rotated" in here, or is the caller responsible for picking up the new yaw?)
}
