#pragma once

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/MetaSetting.hpp"
#include "../../Settings/Tabs.hpp"
#include "../../Setting.hpp"

#include "../../../SDK/Entities/BasePlayerWeapon.hpp"

#include "Elements.hpp"

#include "GenericESP/Element/Element.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>

struct CSPlayerPawn;

class WeaponAmmobar : public PlayerBar {
	using PlayerBar::PlayerBar;

	float get_filled_percentage(const GenericESP::EntityType* e) const override;
	std::string get_number_text_content(const GenericESP::EntityType* e) const override;
};

struct ESPWeapon : public MetaSetting {
	Tabs elements{ this, "Elements" };

	MetaSetting box_meta{ elements, "Box" };
	Checkbox box_enabled{ box_meta, "Enabled", false };
	PlayerRectangle box{ box_meta, "Box" };

	MetaSetting name_meta{ elements, "Name" };
	Checkbox name_enabled{ name_meta, "Enabled", false };
	PlayerSidedText name{ name_meta, "Name" };

	MetaSetting ammobar_meta{ elements, "Ammo bar" };
	Checkbox ammobar_enabled{ ammobar_meta, "Enabled", false };
	WeaponAmmobar ammobar{ ammobar_meta, "Ammo bar" };

	ESPWeapon(SettingsHolder* parent, std::string name);

	// TODO Flags for ammo in case someone prefers the text form.

	bool is_enabled() const;
	void draw_weapon(ImDrawList* draw_list, BasePlayerWeapon* weapon, const ImRect& screenspace_rect) const;
};
