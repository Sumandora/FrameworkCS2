#pragma once

#include "../Schema/SchemaClassInfo.hpp"

struct BaseEntity {
	VIRTUAL_METHOD(35, getSchemaType, SchemaClassInfo*, (), (this));

	static BaseEntity* getLocalPlayer(int index = -1 /*I think this is the splitscreenslot; -1 uses the engine interface to auto-detect*/) {
		return RetAddrSpoofer::invoke<BaseEntity*, int>(Memory::getLocalPlayer, index);
	}
};