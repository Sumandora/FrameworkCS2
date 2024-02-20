#include "CSPlayerWeaponServices.hpp"

Handle<CSWeaponBase> CSPlayerWeaponServices::activeWeapon() const
{
	return PlayerWeaponServices::activeWeapon().as<CSWeaponBase>();
}