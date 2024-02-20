#include "CSPlayerPawnBase.hpp"

#include "../../Services/CSPlayerWeaponServices.hpp"

CSPlayerWeaponServices* CSPlayerPawnBase::weaponServices() const
{
	return static_cast<CSPlayerWeaponServices*>(BasePlayerPawn::weaponServices());
}
