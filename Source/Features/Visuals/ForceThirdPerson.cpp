#include "ForceThirdPerson.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/CSObserverPawn.hpp"
#include "../../SDK/Entities/CSPlayerController.hpp"
#include "../../SDK/Entities/CSPlayerPawnBase.hpp"
#include "../../SDK/Entities/Services/CSPlayerMovementServices.hpp"
#include "../../SDK/Entities/Services/PlayerObserverServices.hpp"
#include "../../SDK/Enums/LifeState.hpp"
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

	CSPlayerPawnBase* view_target = Memory::local_player;

	// TODO this should probably be a common function for multiple features...
	if (view_target->life_state() == LIFE_DEAD)
		if (CSObserverPawn* observer_pawn = (*Memory::local_player_controller)->observer_pawn().get(); observer_pawn)
			if (PlayerObserverServices* observer_services = observer_pawn->observer_services();
				observer_services && observer_services->observer_mode() != OBS_MODE_NONE)
				if (BaseEntity* new_entity = observer_services->observer_target().get(); new_entity)
					if (auto* cs_player = new_entity->entity_cast<CSPlayerPawnBase*>(); cs_player)
						view_target = cs_player;

	const GameSceneNode* game_scene_node = view_target->gameSceneNode();
	glm::vec3 head_pos = game_scene_node->transform().m_Position;

	// This may look hacky, but is preferable to getting the actual head position, as it will suffer from view offsets
	// Source: https://developer.valvesoftware.com/wiki/Counter-Strike:_Global_Offensive/Mapper%27s_Reference
	static constexpr float STANDING_EYE_HEIGHT = 64;
	static constexpr float DUCKING_EYE_HEIGHT = 46;
	head_pos.z += std::lerp(STANDING_EYE_HEIGHT, DUCKING_EYE_HEIGHT, view_target->movement_services()->duck_amount());

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
