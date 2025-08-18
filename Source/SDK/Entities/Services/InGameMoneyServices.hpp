#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "PlayerControllerComponent.hpp"
#include <cstdint>

struct InGameMoneyServices : public PlayerControllerComponent {
	CLASS_INFO("libclient.so", "CCSPlayerController_InGameMoneyServices");

	SCHEMA_VAR(std::int32_t, account, "m_iAccount");
};
