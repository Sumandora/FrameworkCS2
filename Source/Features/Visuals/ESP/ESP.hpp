#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/MetaSetting.hpp"
#include "../../Settings/Tabs.hpp"

#include "../../../Utils/UninitializedObject.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include "Elements.hpp"

#include "imgui_internal.h"

#include <atomic>
#include <string>

struct CSPlayerPawn;

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled", false };

	Tabs entity_types{ this, "Entity types" };

	Tabs players{ entity_types, "Players" };

	struct Player : public MetaSetting {
		using MetaSetting::MetaSetting;

		Tabs elements{ this, "Elements" };

		MetaSetting box_meta{ elements, "Box" };
		Checkbox box_enabled{ box_meta, "Enabled", false };
		PlayerRectangle box{ box_meta, "Box" };

		MetaSetting name_meta{ elements, "Name" };
		Checkbox name_enabled{ name_meta, "Enabled", false };
		PlayerSidedText name{ name_meta, "Name" };

		MetaSetting healthbar_meta{ elements, "Health bar" };
		Checkbox healthbar_enabled{ healthbar_meta, "Enabled", false };
		PlayerHealthbar healthbar{ healthbar_meta, "Health bar" };

		MetaSetting skeleton_meta{ elements, "Skeleton" };
		Checkbox skeleton_enabled{ skeleton_meta, "Enabled", false };
		PlayerLine skeleton_line{ skeleton_meta, "Line" };

		Checkbox autowall_debug{ skeleton_meta, "Autowall debug", false };

		bool is_enabled() const;
		void draw_player(ImDrawList* draw_list, CSPlayerPawn* player_pawn, const ImRect& screenspace_rect) const;
	};

	Player teammates{ players, "Teammates" };
	Player enemies{ players, "Enemies" };
	Player local{ players, "Local" };

	Player& get_player_by_pawn(CSPlayerPawn* player_pawn);
	
	std::atomic<glm::vec3> camera_position;

public:
	ESP();

	void update_camera_position(const glm::vec3& new_camera_position);

	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<ESP> esp;
