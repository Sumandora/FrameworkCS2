#pragma once

#include "../../Schema/FieldOffset.hpp"
#include "../../Schema/SchemaClassInfo.hpp"
#include "CSPlayerPawnBase.hpp"

struct CSPlayerPawn : CSPlayerPawnBase {
	CLASS_INFO("libclient.so", "C_CSPlayerPawn");
};