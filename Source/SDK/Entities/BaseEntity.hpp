#pragma once

#include "../Schema/SchemaClassInfo.hpp"
#include "../Schema/FieldOffset.hpp"

struct CollisionProperty;
struct GameSceneNode;

struct BaseEntity {
	VIRTUAL_METHOD(35, getSchemaType, SchemaClassInfo*, (), (this));

	CLASS_INFO("libclient.so", "C_BaseEntity");

	SCHEMA_VAR(GameSceneNode*, gameSceneNode, "m_pGameSceneNode");
	SCHEMA_VAR(CollisionProperty*, collision, "m_pCollision");

	static BaseEntity* getLocalPlayer(int index = -1 /*I think this is the splitscreenslot; -1 uses the engine interface to auto-detect*/) {
		return RetAddrSpoofer::invoke<BaseEntity*, int>(Memory::getLocalPlayer, index);
	}
};