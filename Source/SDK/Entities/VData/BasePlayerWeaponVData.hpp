#pragma once

#include "../../Schema/FieldOffset.hpp"

#include "EntitySubclassVDataBase.hpp"
#include <cstdint>

struct BasePlayerWeaponVData : public EntitySubclassVDataBase {
	CLASS_INFO("libclient.so", "CBasePlayerWeaponVData");

	SCHEMA_VAR(std::int32_t, max_clip1, "m_iMaxClip1");
};
