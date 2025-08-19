#pragma once

#include "BaseModelEntity.hpp"

#include "../Schema/FieldOffset.hpp"

struct World : public BaseModelEntity {
	CLASS_INFO("libclient.so", "C_World");
};
