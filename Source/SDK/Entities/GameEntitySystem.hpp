#pragma once

#include "../../Memory.hpp"
#include "RetAddrSpoofer.hpp"

struct BaseEntity;

class GameEntitySystem {
	static int (*get_highest_entity_index_ptr)(GameEntitySystem* thisptr);
	static BaseEntity* (*get_base_entity_ptr)(GameEntitySystem* thisptr, int index);

	static GameEntitySystem** find();

public:
	static GameEntitySystem* the();

	int getHighestEntityIndex()
	{
		return RetAddrSpoofer::invoke(get_highest_entity_index_ptr, this);
	}

	BaseEntity* getBaseEntity(int index)
	{
		return RetAddrSpoofer::invoke(get_base_entity_ptr, this, index);
	}
};
