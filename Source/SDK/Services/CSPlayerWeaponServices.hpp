#pragma once

#include "../Entities/Weapon/CSWeaponBase.hpp"
#include "../GameClass/Handle.hpp"
#include "../Schema/FieldOffset.hpp"
#include "PlayerWeaponServices.hpp"

struct CSPlayerWeaponServices : PlayerWeaponServices {
	CLASS_INFO("libclient.so", "CCSPlayer_WeaponServices");

	[[nodiscard]] Handle<CSWeaponBase> activeWeapon() const;
};