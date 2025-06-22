#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../EntityHandle.hpp"

#include "BasePlayerPawn.hpp"

struct CSPlayerController;

struct CSPlayerPawnBase : public BasePlayerPawn {
	CLASS_INFO("libclient.so", "C_CSPlayerPawnBase");

	SCHEMA_VAR(EntityHandle<CSPlayerController>, original_controller, "m_hOriginalController")
};
