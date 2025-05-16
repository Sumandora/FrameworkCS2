#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"
#include "CSPlayerPawnBase.hpp"

struct CSPlayerPawn : public CSPlayerPawnBase {
	CLASS_INFO("libclient.so", "C_CSPlayerPawn");

	SCHEMA_VAR(bool, isScoped, "m_bIsScoped");
};
