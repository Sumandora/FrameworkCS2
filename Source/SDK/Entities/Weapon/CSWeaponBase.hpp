#pragma once

#include <string>

#include "../../Schema/FieldOffset.hpp"
#include "../../Schema/SchemaClassInfo.hpp"
#include "../../VData/CSWeaponBaseVData.hpp"
#include "BasePlayerWeapon.hpp"

struct CSWeaponBase : BasePlayerWeapon {
	CLASS_INFO("libclient.so", "C_CSWeaponBase");

	SCHEMA_VAR(bool, inReload, "m_bInReload")

	[[nodiscard]] CSWeaponBaseVData* getWeaponVData() const;
};