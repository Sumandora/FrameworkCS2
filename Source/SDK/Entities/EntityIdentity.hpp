#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../Padding.hpp"

#include <cstddef>
#include <cstdint>

struct EntityIdentity {
	CLASS_INFO("libclient.so", "CEntityIdentity");

	PADDING(0x10);
	std::uint32_t entry_index;

	SCHEMA_VAR(std::uint32_t, flags, "m_flags");
};

static_assert(offsetof(EntityIdentity, entry_index) == 0x10);
