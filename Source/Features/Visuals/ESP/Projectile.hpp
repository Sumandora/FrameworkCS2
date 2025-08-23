#pragma once

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/MetaSetting.hpp"
#include "../../Settings/Tabs.hpp"
#include "../../Setting.hpp"

#include "../../../SDK/Entities/BaseCSGrenadeProjectile.hpp"

#include "Elements.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <string_view>

struct CSPlayerPawn;

struct ESPProjectile : public MetaSetting {
	Tabs elements{ this, "Elements" };

	MetaSetting box_meta{ elements, "Box" };
	Checkbox box_enabled{ box_meta, "Enabled", false };
	PlayerRectangle box{ box_meta, "Box" };

	MetaSetting name_meta{ elements, "Name" };
	Checkbox name_enabled{ name_meta, "Enabled", false };
	PlayerSidedText name{ name_meta, "Name" };

	MetaSetting thrower_meta{ elements, "Thrower" };
	Checkbox thrower_enabled{ thrower_meta, "Enabled", false };
	PlayerSidedText thrower{ thrower_meta, "Thrower" };

	// TODO Trail

	const char* localization_key;

	ESPProjectile(SettingsHolder* parent, std::string name, const char* localization_key);

	bool is_enabled() const;
	void draw_projectile(ImDrawList* draw_list, BaseCSGrenadeProjectile* projectile, const ImRect& screenspace_rect) const;
};
