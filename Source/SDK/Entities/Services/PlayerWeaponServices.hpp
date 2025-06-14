#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "../../EntityHandle.hpp"

struct BasePlayerWeapon;

struct PlayerWeaponServices {
    CLASS_INFO("libclient.so", "CPlayer_WeaponServices")

    SCHEMA_VAR(EntityHandle<BasePlayerWeapon>, active_weapon, "m_hActiveWeapon")
};
