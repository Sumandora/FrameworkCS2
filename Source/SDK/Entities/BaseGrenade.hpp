#pragma once

#include "../EntityHandle.hpp"
#include "../Schema/FieldOffset.hpp"
#include "BaseFlex.hpp"
#include "CSPlayerPawn.hpp"

struct BaseGrenade : public BaseFlex {
	CLASS_INFO("libclient.so", "C_BaseGrenade");

	SCHEMA_VAR(EntityHandle<CSPlayerPawn>, thrower, "m_hThrower");
};
