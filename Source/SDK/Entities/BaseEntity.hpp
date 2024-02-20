#pragma once

#include "../Schema/SchemaClassInfo.hpp"
#include "../Schema/FieldOffset.hpp"

#include "../../Memory.hpp"

struct CollisionProperty;
struct GameSceneNode;
struct CSPlayerPawnBase;
struct CUtlStringToken;

struct BaseEntity {
	VIRTUAL_METHOD(37, getSchemaType, SchemaClassInfo*, (), (this));

	CLASS_INFO("libclient.so", "C_BaseEntity");

	SCHEMA_VAR(GameSceneNode*, gameSceneNode, "m_pGameSceneNode")
	SCHEMA_VAR(CollisionProperty*, collision, "m_pCollision")
	SCHEMA_VAR(std::int32_t, health, "m_iHealth")
    SCHEMA_VAR(CUtlStringToken, subclassID, "m_nSubclassID")

	static CSPlayerPawnBase* /*TODO find actual type*/ getLocalPlayer(int index = -1 /*I think this is the splitscreenslot; -1 uses the engine interface to auto-detect*/) {
		return RetAddrSpoofer::invoke<CSPlayerPawnBase*, int>(Memory::getLocalPlayer, index);
	}

   protected:
    // call in inheriting classes and cast to the correct type
    SCHEMA_VAR_OFFSET(void*, getVData, subclassID, 0x8)
};