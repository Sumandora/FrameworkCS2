#pragma once

#include "../../Memory.hpp"
#include "RetAddrSpoofer.hpp"

#include "BaseEntity.hpp"

class GameEntitySystem {
	static void* get_highest_entity_index_ptr;
	static void* get_base_entity_ptr;

	static GameEntitySystem** find();
public:
	static GameEntitySystem* the();

	int getHighestEntityIndex()
	{
		return RetAddrSpoofer::invoke<int, void*>(get_highest_entity_index_ptr, this);
	}

	BaseEntity* getBaseEntity(int index)
	{
		return RetAddrSpoofer::invoke<BaseEntity*, void*, int>(get_base_entity_ptr, this, index);
	}
};
