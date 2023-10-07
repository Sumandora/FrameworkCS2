#pragma once

#include "../../Memory.hpp"
#include "RetAddrSpoofer.hpp"

#include "BaseEntity.hpp"

struct GameEntitySystem { // TODO Reorganize the entire structure of this
	inline int getHighestEntityIndex() {
		return RetAddrSpoofer::invoke<int, void*>(Memory::EntitySystem::getHighestEntityIndex, this);
	}

	inline BaseEntity* getBaseEntity(int index) {
		return RetAddrSpoofer::invoke<BaseEntity*, void*, int>(Memory::EntitySystem::getBaseEntity, this, index);
	}
};