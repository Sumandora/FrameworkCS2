#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../CUtl/SymbolLarge.hpp"

#include "../Padding.hpp"

#include <cstddef>
#include <cstdint>

struct EntityInstance;

struct EntityIdentity {
	CLASS_INFO("libclient.so", "CEntityIdentity");

	EntityInstance* instance;
	PADDING(0x8);
	std::uint32_t entry_index;

	SCHEMA_VAR(UtlSymbolLarge, designer_name, "m_designerName");

	SCHEMA_VAR(std::uint32_t, flags, "m_flags");

	SCHEMA_VAR(EntityIdentity*, prev, "m_pPrev");
	SCHEMA_VAR(EntityIdentity*, next, "m_pNext");
	SCHEMA_VAR(EntityIdentity*, prev_by_class, "m_pPrevByClass");
	SCHEMA_VAR(EntityIdentity*, next_by_class, "m_pNextByClass");
};

static_assert(offsetof(EntityIdentity, entry_index) == 0x10);
