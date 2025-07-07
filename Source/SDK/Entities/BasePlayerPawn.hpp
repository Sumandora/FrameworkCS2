#pragma once

#include "../Schema/FieldOffset.hpp"

#include "BaseModelEntity.hpp"

#include "glm/ext/vector_float3.hpp"

struct PlayerMovementServices;
struct PlayerWeaponServices;
struct PlayerItemServices;

struct BasePlayerPawn : public BaseModelEntity {
	CLASS_INFO("libclient.so", "C_BasePlayerPawn");

	SCHEMA_VAR(glm::vec3, old_origin, "m_vOldOrigin");

	SCHEMA_VAR(PlayerWeaponServices*, weapon_services, "m_pWeaponServices");
	SCHEMA_VAR(PlayerMovementServices*, movement_services, "m_pMovementServices");
	SCHEMA_VAR(PlayerItemServices*, item_services, "m_pItemServices");
};
