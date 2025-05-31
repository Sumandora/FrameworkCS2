#pragma once

#include "BaseEntity.hpp"
#include "../EntityHandle.hpp"

struct CSPlayerController;

struct CSPlayerPawnBase : public BaseEntity {
	CLASS_INFO("libclient.so", "C_CSPlayerPawnBase");

	SCHEMA_VAR(EntityHandle<CSPlayerController>, original_controller, "m_hOriginalController")
};
