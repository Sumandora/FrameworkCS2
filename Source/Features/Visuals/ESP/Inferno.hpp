#pragma once

#include "../../Setting.hpp"
#include "../../Settings/Checkbox.hpp"
#include "../../Settings/MetaSetting.hpp"
#include "../../Settings/Tabs.hpp"

#include "../../../SDK/Entities/Inferno.hpp"

#include "Elements.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>

struct CSPlayerPawn;

struct ESPInferno : public MetaSetting {
	Tabs elements{ this, "Elements" };

	MetaSetting box_meta{ elements, "Box" };
	Checkbox box_enabled{ box_meta, "Enabled", false };
	PlayerRectangle box{ box_meta, "Box" };

	MetaSetting name_meta{ elements, "Name" };
	Checkbox name_enabled{ name_meta, "Enabled", false };
	PlayerSidedText name{ name_meta, "Name" };

	MetaSetting convex_hull_meta{ elements, "Convex hull" };
	Checkbox convex_hull_enabled{ convex_hull_meta, "Enabled", false };
	PlayerPolygon convex_hull{ convex_hull_meta, "Convex hull" };

	ESPInferno(SettingsHolder* parent, std::string name);

	bool is_enabled() const;
	void draw_inferno(ImDrawList* draw_list, Inferno* inferno, const ImRect& screenspace_rect) const;
};
