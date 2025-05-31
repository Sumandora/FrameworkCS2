#pragma once

#include "../EntityHandle.hpp"
#include "../Schema/FieldOffset.hpp"
#include "BaseEntity.hpp"

struct CSPlayerPawnBase;

struct BasePlayerController : BaseEntity {
	CLASS_INFO("libclient.so", "CBasePlayerController");

	SCHEMA_VAR(EntityHandle<CSPlayerPawnBase>, pawn, "m_hPawn");
};
