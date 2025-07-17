#include "ForceThirdPerson.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/Entities/Services/CSPlayerMovementServices.hpp"
#include "../../SDK/GameClass/GameSceneNode.hpp"
#include "../../SDK/GameClass/ViewSetup.hpp"

#include "../../Memory.hpp"

#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <cmath>

ForceThirdPerson::ForceThirdPerson()
	: Feature("Visuals", "Force third person")
{
}

void ForceThirdPerson::update_viewangles(const glm::vec3& view_angles)
{
	this->view_angles = view_angles;
}

void ForceThirdPerson::override_view(ViewSetup* view_setup)
{
	if (!enabled.get())
		return;

	if (!Memory::local_player)
		return;

	const GameSceneNode* game_scene_node = Memory::local_player->gameSceneNode();
	glm::vec3 head_pos = game_scene_node->transform().m_Position;

	const CSPlayerMovementServices* movement_services = static_cast<CSPlayerMovementServices*>(Memory::local_player->movement_services());

	// This may look hacky, but is preferable to getting the actual head position, as it will suffer from view offsets
	// Source: https://developer.valvesoftware.com/wiki/Counter-Strike:_Global_Offensive/Mapper%27s_Reference
	static constexpr float STANDING_EYE_HEIGHT = 64;
	static constexpr float DUCKING_EYE_HEIGHT = 46;
	head_pos.z += std::lerp(STANDING_EYE_HEIGHT, DUCKING_EYE_HEIGHT, movement_services ? movement_services->duck_amount() : 0.0F);

	const float pitch = glm::radians(view_angles.x);
	const float yaw = glm::radians(view_angles.y);
	const glm::vec3 dir{
		glm::cos(yaw) * glm::cos(pitch),
		glm::sin(yaw) * glm::cos(pitch),
		-glm::sin(pitch),
	};

	// TODO: (Setting to) prevent camera from clipping

	view_setup->camera_position() = head_pos + -dir * distance.get();
	view_setup->view_angles() = view_angles;
}
