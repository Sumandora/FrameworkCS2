#include "CSWeaponBase.hpp"

CSWeaponBaseVData* CSWeaponBase::getWeaponVData() const
{
	return static_cast<CSWeaponBaseVData*>(getVData());
}