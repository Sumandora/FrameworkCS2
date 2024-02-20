#include "EconItemView.hpp"

#include "WeaponID.hpp"

const std::string& EconItemView::getName() const
{
	return getWeaponName(static_cast<WeaponID>(itemDefinitionIndex()));
}
