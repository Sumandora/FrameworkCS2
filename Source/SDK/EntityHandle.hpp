#pragma once

#include "Entities/GameEntitySystem.hpp"

#include <cstddef>
#include <cstdint>

static constexpr std::size_t ENTITY_SERIAL_BITS_COUNT = 15;
static constexpr std::uint32_t ENTITY_LIST_INDEX_MASK = (1 << ENTITY_SERIAL_BITS_COUNT) - 1;
static constexpr std::uint32_t INVALID_ENTITY_HANDLE = 0xFFFFFFFF;

template <typename Ent>
class EntityHandle /* a.k.a. CBaseHandle and CHandle */ {
	std::uint32_t index;

public:
	explicit EntityHandle(std::uint32_t index)
		: index(index)
	{
	}

	static EntityHandle invalid()
	{
		return EntityHandle{ INVALID_ENTITY_HANDLE };
	}

	[[nodiscard]] Ent* get() const
	{
		if (!has_entity())
			return nullptr;

		return static_cast<Ent*>(GameEntitySystem::the()->get_entity_by_index(index & ENTITY_LIST_INDEX_MASK));
	}

	[[nodiscard]] int get_index() const
	{
		return index & ENTITY_LIST_INDEX_MASK;
	}

	[[nodiscard]] bool has_entity() const
	{
		return index != INVALID_ENTITY_HANDLE;
	}

	[[nodiscard]] std::uint32_t raw() const
	{
		return index;
	}

	auto operator<=>(const EntityHandle<Ent>& other) const
	{
		return index <=> other.index;
	}
};
