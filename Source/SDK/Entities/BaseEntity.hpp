#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"
#include "../Schema/SchemaSystem.hpp"

#include "../EntityHandle.hpp"

#include "../../Memory.hpp"

#include "../Enums/LifeState.hpp"

#include "EntityInstance.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstdint>

struct CollisionProperty;
struct GameSceneNode;
struct CSPlayerPawn;
struct BodyComponent;
struct EntitySubclassVDataBase;

constexpr std::uint32_t FL_ONGROUND = 1 << 0;
constexpr std::uint32_t FL_DUCKING = 1 << 1;

// NOLINTNEXTLINE(performance-enum-size)
enum class TeamID : int {
	TEAM_INVALID = -1,
	TEAM_ANY = -1,
	TEAM_UNASSIGNED = 0,
	TEAM_SPECTATOR = 1,
	TEAM_TERRORIST = 2,
	TEAM_COUNTER_TERRORIST = 3,
};

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

	SCHEMA_VAR(EntityHandle<BaseEntity>, owner_entity, "m_hOwnerEntity");

	SCHEMA_VAR(TeamID, team_id, "m_iTeamNum");

	SCHEMA_VAR(glm::vec3, velocity, "m_vecAbsVelocity");

	// This type changes based on the entity type
	EntitySubclassVDataBase* get_vdata()
	{
		static const std::int32_t OFFSET = SchemaSystem::findOffset(classInfo(), "m_nSubclassID");
		return *reinterpret_cast<EntitySubclassVDataBase**>(reinterpret_cast<char*>(this) + OFFSET + 0x8);
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

	[[nodiscard]] EntityHandle<BaseEntity> get_handle() const;

	[[nodiscard]] bool is_enemy(BaseEntity* other) const;
};
