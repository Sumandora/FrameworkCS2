#pragma once

#include "../EntityHandle.hpp"
#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaSystem.hpp"
#include "BaseEntity.hpp"

#include <cstdint>

struct CSPlayerPawnBase;
struct UserCmd;

struct BasePlayerController : BaseEntity {
	CLASS_INFO("libclient.so", "CBasePlayerController");

	SCHEMA_VAR(EntityHandle<CSPlayerPawnBase>, pawn, "m_hPawn");

	SCHEMA_VAR(std::uint32_t, tick_base, "m_nTickBase");

	UserCmd** get_current_command()
	{
		static const std::int32_t OFFSET = SchemaSystem::findOffset(classInfo(), "m_steamID");
		return reinterpret_cast<UserCmd**>(reinterpret_cast<char*>(this) + OFFSET - 0x8);
	}
};
