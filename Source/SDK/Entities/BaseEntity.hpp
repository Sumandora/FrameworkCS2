#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"
#include "../Schema/SchemaSystem.hpp"

#include "../../Memory.hpp"

#include "../Enums/LifeState.hpp"

#include "EntityInstance.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstdint>
#include <type_traits>

struct CollisionProperty;
struct GameSceneNode;
struct CSPlayerPawn;
struct BodyComponent;
struct EntitySubclassVDataBase;

template <typename Ent>
class EntityHandle;

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

// @schema MoveType_t
enum MoveType : std::uint8_t {
	MOVETYPE_NONE = 0,
	MOVETYPE_OBSOLETE = 1,
	MOVETYPE_WALK = 2,
	MOVETYPE_FLY = 3,
	MOVETYPE_FLYGRAVITY = 4,
	MOVETYPE_VPHYSICS = 5,
	MOVETYPE_PUSH = 6,
	MOVETYPE_NOCLIP = 7,
	MOVETYPE_OBSERVER = 8,
	MOVETYPE_LADDER = 9,
	MOVETYPE_CUSTOM = 10,
	MOVETYPE_LAST = 11,
	MOVETYPE_INVALID = 11,
	MOVETYPE_MAX_BITS = 5,
};

struct BaseEntity : public EntityInstance {
	VIRTUAL_METHOD(43, getSchemaType, SchemaClassInfo*, (), (this));

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

	SCHEMA_VAR(MoveType, move_type, "m_nActualMoveType");

	SCHEMA_VAR(float, simulation_time, "m_flSimulationTime");

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

	[[nodiscard]] EntityHandle<BaseEntity> get_raw_handle() const;

	template <typename Self>
	auto get_handle(this const Self& self) -> EntityHandle<Self>
	{
		return EntityHandle<Self>{ self.get_raw_handle().raw() };
	}

	[[nodiscard]] bool is_enemy(BaseEntity* other) const;
};
