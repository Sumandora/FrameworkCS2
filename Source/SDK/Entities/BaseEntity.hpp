#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"
#include "../Schema/SchemaSystem.hpp"

#include "../VirtualMethod.hpp"

#include "../Enums/Flags.hpp"
#include "../Enums/LifeState.hpp"
#include "../Enums/MoveType.hpp"
#include "../Enums/TeamID.hpp"

#include "EntityInstance.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstdint>

struct CollisionProperty;
struct GameSceneNode;
struct CSPlayerPawn;
struct BodyComponent;
struct EntitySubclassVDataBase;

template <typename Ent>
class EntityHandle;

struct BaseEntity : public EntityInstance {
	VIRTUAL_METHOD(43, getSchemaType, SchemaClassInfo*, () const, (this));

	CLASS_INFO("libclient.so", "C_BaseEntity");

	SCHEMA_VAR(BodyComponent*, body_component, "m_CBodyComponent");

	SCHEMA_VAR(GameSceneNode*, gameSceneNode, "m_pGameSceneNode");
	SCHEMA_VAR(CollisionProperty*, collision, "m_pCollision");

	SCHEMA_VAR(LifeState, life_state, "m_lifeState");

	SCHEMA_VAR(std::int32_t, health, "m_iHealth");
	SCHEMA_VAR(std::int32_t, max_health, "m_iMaxHealth");

	SCHEMA_VAR(Flags, flags, "m_fFlags");

	SCHEMA_VAR(EntityHandle<BaseEntity>, owner_entity, "m_hOwnerEntity");

private:
	SCHEMA_VAR(std::uint8_t, raw_team_id, "m_iTeamNum");

public:
	[[nodiscard]] TeamID team_id() const
	{
		return static_cast<TeamID>(raw_team_id());
	}

	SCHEMA_VAR(glm::vec3, abs_velocity, "m_vecAbsVelocity");
	SCHEMA_VAR(glm::vec3, velocity, "m_vecVelocity");

	SCHEMA_VAR(MoveType, move_type, "m_nActualMoveType");

	SCHEMA_VAR(float, simulation_time, "m_flSimulationTime");

	// This type changes based on the entity type
	[[nodiscard]] EntitySubclassVDataBase* get_vdata() const
	{
		static const std::int32_t OFFSET = SchemaSystem::findOffset(classInfo(), "m_nSubclassID");
		return *reinterpret_cast<EntitySubclassVDataBase* const*>(reinterpret_cast<const char*>(this) + OFFSET + 0x8);
	}

	template <typename T>
	T entity_cast()
	{
		return schema_cast<T>(this, getSchemaType());
	}

	[[nodiscard]] bool alive() const
	{
		return life_state() == LIFE_ALIVE && health() > 0;
	}

	[[nodiscard]] EntityHandle<BaseEntity> get_raw_handle() const;

	template <typename Self>
	auto get_handle(this const Self& self) -> EntityHandle<Self>
	{
		return EntityHandle<Self>{ self.get_raw_handle().raw() };
	}

	[[nodiscard]] bool is_enemy(BaseEntity* other) const;
};
