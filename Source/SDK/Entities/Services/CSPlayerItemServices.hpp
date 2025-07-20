#pragma once

#include "PlayerItemServices.hpp"

#include "../../Schema/FieldOffset.hpp"

struct CSPlayerItemServices : public PlayerItemServices {
	CLASS_INFO("libclient.so", "CCSPlayer_ItemServices");

	SCHEMA_VAR(bool, has_defuser, "m_bHasDefuser");
	SCHEMA_VAR(bool, has_helmet, "m_bHasHelmet");
};
