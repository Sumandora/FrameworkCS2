#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BasePlayerController.hpp"

struct CSPlayerController : BasePlayerController {
	CLASS_INFO("libclient.so", "CCSPlayerController");

	SCHEMA_VAR(const char*, sanitizied_name, "m_sSanitizedPlayerName");
};
