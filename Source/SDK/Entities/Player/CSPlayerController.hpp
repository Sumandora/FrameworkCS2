#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "../../Schema/SchemaClassInfo.hpp"
#include "../BaseEntity.hpp"

struct CSPlayerController : BaseEntity {
	CLASS_INFO("libclient.so", "CCSPlayerController")

	SCHEMA_VAR(const char*, sanitizedPlayerName, "m_sSanitizedPlayerName")
};