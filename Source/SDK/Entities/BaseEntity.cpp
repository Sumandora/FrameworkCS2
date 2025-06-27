#include "BaseEntity.hpp"

#include "../EntityHandle.hpp"

#include "EntityIdentity.hpp"

#include <cstdint>

EntityHandle<BaseEntity> BaseEntity::get_handle()
{
	if (EntityIdentity* identity = entity(); identity) {
		std::uint16_t entry_index = 0;
		if (identity->entry_index == INVALID_ENTITY_HANDLE) {
			entry_index = 0x7fff;
		} else {
			entry_index = identity->entry_index & ENTITY_LIST_INDEX_MASK;
		}
		const std::uint32_t index = ((identity->entry_index >> ENTITY_SERIAL_BITS_COUNT) - (identity->flags() & 1)) * 0x8000 | static_cast<uint32_t>(entry_index);
		return EntityHandle<BaseEntity>{ index };
	}

	return EntityHandle<BaseEntity>::invalid();
}
