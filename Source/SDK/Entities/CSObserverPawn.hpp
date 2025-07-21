#pragma once

#include "../Schema/FieldOffset.hpp"

#include "CSPlayerPawnBase.hpp"

struct CSObserverPawn : public CSPlayerPawnBase {
	CLASS_INFO("libclient.so", "C_CSObserverPawn");
};
