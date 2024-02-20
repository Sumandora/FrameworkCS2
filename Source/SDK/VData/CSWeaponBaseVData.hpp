#pragma once

#include "BasePlayerWeaponVData.hpp"

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"

struct CSWeaponBaseVData : BasePlayerWeaponVData {
	CLASS_INFO("libclient.so", "CCSWeaponBaseVData")
};