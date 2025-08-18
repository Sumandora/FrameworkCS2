#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/Tabs.hpp"

#include "../../../Utils/UninitializedObject.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include "Player.hpp"
#include "Weapon.hpp"

#include <atomic>
#include <string>

struct CSPlayerPawn;

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled", false };

	Tabs entity_types{ this, "Entity types" };

	Tabs players{ entity_types, "Players" };
	ESPPlayer enemies{ players, "Enemies" };
	ESPPlayer teammates{ players, "Teammates" };
	ESPPlayer local{ players, "Local" };

	ESPWeapon weapons{ entity_types, "Weapons" };

	std::atomic<glm::vec3> camera_position;

	friend ESPPlayer;

	ESPPlayer& get_player_by_pawn(CSPlayerPawn* player_pawn);

public:
	ESP();

	void update_camera_position(const glm::vec3& new_camera_position);

	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<ESP> esp;
