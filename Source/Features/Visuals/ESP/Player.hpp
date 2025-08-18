#pragma once

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/MetaSetting.hpp"
#include "../../Settings/Tabs.hpp"

#include "Elements.hpp"

#include "imgui.h"
#include "imgui_internal.h"

struct CSPlayerPawn;

struct ESPPlayer : public MetaSetting {
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

	bool is_enabled() const;
	void draw_player(ImDrawList* draw_list, CSPlayerPawn* player_pawn, const ImRect& screenspace_rect) const;
};
