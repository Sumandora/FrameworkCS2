#pragma once

#include <cstdint>

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"

struct BasePlayerWeaponVData {
	CLASS_INFO("libclient.so", "CBasePlayerWeaponVData")

	SCHEMA_VAR(std::int32_t, maxClip1, "m_iMaxClip1")
};