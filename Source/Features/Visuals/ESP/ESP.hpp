#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/Tabs.hpp"

#include "../../../Utils/UninitializedObject.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include "Player.hpp"

#include <atomic>
#include <string>

struct CSPlayerPawn;

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled", false };

	Tabs entity_types{ this, "Entity types" };

	Tabs players{ entity_types, "Players" };

	ESPPlayer teammates{ players, "Teammates" };
	ESPPlayer enemies{ players, "Enemies" };
	ESPPlayer local{ players, "Local" };

	friend ESPPlayer;

	ESPPlayer& get_player_by_pawn(CSPlayerPawn* player_pawn);

	std::atomic<glm::vec3> camera_position;

public:
	ESP();

	void update_camera_position(const glm::vec3& new_camera_position);

	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<ESP> esp;
