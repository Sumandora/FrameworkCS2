#pragma once

#include "Entities/GameEntitySystem.hpp"

#include <cstddef>
#include <cstdint>

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
		return EntityHandle{ GameEntitySystem::INVALID_ENTITY_HANDLE };
	}

	[[nodiscard]] Ent* get() const
	{
		if (!has_entity())
			return nullptr;

		return static_cast<Ent*>(GameEntitySystem::the()->get_entity_by_index(index & GameEntitySystem::ENTITY_LIST_INDEX_MASK));
	}

	[[nodiscard]] int get_index() const
	{
		return index & GameEntitySystem::ENTITY_LIST_INDEX_MASK;
	}

	[[nodiscard]] bool has_entity() const
	{
		return index != GameEntitySystem::INVALID_ENTITY_HANDLE;
	}

	[[nodiscard]] std::uint32_t raw() const
	{
		return index;
	}

	auto operator<=>(const EntityHandle<Ent>& other) const
	{
		return index <=> other.index;
	}

	bool operator==(const EntityHandle<Ent>& other) const {
		return index == other.index;
	}
};
