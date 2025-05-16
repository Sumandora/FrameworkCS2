#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"

#include "../../Memory.hpp"

#include "../GameClass/EngineToClient.hpp"
#include "CSPlayerPawn.hpp"
#include "RetAddrSpoofer.hpp"

struct CollisionProperty;
struct GameSceneNode;
struct CSPlayerPawnBase;

struct BaseEntity {
	VIRTUAL_METHOD(39, getSchemaType, SchemaClassInfo*, (), (this));

	CLASS_INFO("libclient.so", "C_BaseEntity");

	SCHEMA_VAR(GameSceneNode*, gameSceneNode, "m_pGameSceneNode");
	SCHEMA_VAR(CollisionProperty*, collision, "m_pCollision");

	static CSPlayerPawnBase* /*TODO find actual type*/ getLocalPlayer()
	{
		return RetAddrSpoofer::invoke<CSPlayerPawnBase*>(Memory::getLocalPlayer);
	}
};
