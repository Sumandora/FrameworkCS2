#pragma once

#include "../Schema/SchemaClassInfo.hpp"
#include "../Schema/FieldOffset.hpp"

struct CollisionProperty;
struct CGameSceneNode;

struct BaseEntity {
	VIRTUAL_METHOD(35, getSchemaType, SchemaClassInfo*, (), (this));

	CLASS_INFO("libclient.so", "C_BaseEntity");

	SCHEMA_VAR(CGameSceneNode*, gameSceneNode, "m_pGameSceneNode");
	SCHEMA_VAR(CollisionProperty*, collision, "m_pCollision");
};