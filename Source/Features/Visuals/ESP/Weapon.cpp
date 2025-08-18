#include "Weapon.hpp"

#include "../../Setting.hpp"
#include "../../Settings/MetaSetting.hpp"

#include "../../../SDK/Entities/BasePlayerWeapon.hpp"
#include "../../../SDK/Entities/VData/BasePlayerWeaponVData.hpp"

#include "GenericESP/Element/Element.hpp"
#include "GenericESP/Element/SidedElement.hpp"
#include "GenericESP/UnionedRect.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <utility>

float WeaponAmmobar::get_filled_percentage(const GenericESP::EntityType* e) const
{
	const auto* weapon = static_cast<const BasePlayerWeapon*>(e);
	return static_cast<float>(weapon->clip1()) / static_cast<float>(weapon->get_vdata()->max_clip1());
}

std::string WeaponAmmobar::get_number_text_content(const GenericESP::EntityType* e) const
{
	const auto* weapon = static_cast<const BasePlayerWeapon*>(e);
	return std::to_string(weapon->clip1());
}

ESPWeapon::ESPWeapon(SettingsHolder* parent, std::string name)
	: MetaSetting(parent, std::move(name))
{
	ammobar.filled_color.set(ImColor{ 0.0F, 0.5F, 1.0F, 1.0F });
	ammobar.empty_color.set(ImColor{ 0.0F, 0.5F, 1.0F, 1.0F });
	ammobar.side.set(GenericESP::Side::BOTTOM);
}

bool ESPWeapon::is_enabled() const
{
	return box_enabled.get() || name_enabled.get() || ammobar_enabled.get();
}

void ESPWeapon::draw_weapon(ImDrawList* draw_list, BasePlayerWeapon* weapon, const ImRect& screenspace_rect) const
{
	GenericESP::UnionedRect unioned_rect{ screenspace_rect };

	if (box_enabled.get())
		box.draw(draw_list, weapon, unioned_rect);

	if (name_enabled.get()) {
		const std::string name = weapon->get_display_name();
		this->name.draw(draw_list, weapon, name, unioned_rect);
	}

	if (ammobar_enabled.get())
		ammobar.draw(draw_list, weapon, unioned_rect);
}
