#pragma once

#include "BaseEntity.hpp"

struct CSPlayerPawnBase : public BaseEntity {
	CLASS_INFO("libclient.so", "C_CSPlayerPawnBase");

	SCHEMA_VAR(bool, isScoped, "m_bIsScoped");
};