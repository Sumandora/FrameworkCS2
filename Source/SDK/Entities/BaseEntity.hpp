#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"

#include "../EntityHandle.hpp"

#include "../../Memory.hpp"

#include "../Enums/LifeState.hpp"

#include "EntityInstance.hpp"

#include <cstdint>

struct CollisionProperty;
struct GameSceneNode;
struct CSPlayerPawn;
struct BodyComponent;

struct BaseEntity : public EntityInstance {
	VIRTUAL_METHOD(39, getSchemaType, SchemaClassInfo*, (), (this));

	CLASS_INFO("libclient.so", "C_BaseEntity");

	SCHEMA_VAR(BodyComponent*, body_component, "m_CBodyComponent");

	SCHEMA_VAR(GameSceneNode*, gameSceneNode, "m_pGameSceneNode");
	SCHEMA_VAR(CollisionProperty*, collision, "m_pCollision");

	SCHEMA_VAR(LifeState, life_state, "m_lifeState");

	SCHEMA_VAR(std::int32_t, health, "m_iHealth");
	SCHEMA_VAR(std::int32_t, max_health, "m_iMaxHealth");

	SCHEMA_VAR(std::uint32_t, flags, "m_fFlags");

	SCHEMA_VAR(EntityHandle<BaseEntity>, owner_entity, "m_hOwnerEntity")

	template <typename T>
	T entity_cast()
	{
		return schema_cast<T>(this, getSchemaType());
	}

	EntityHandle<BaseEntity> get_handle();
};
