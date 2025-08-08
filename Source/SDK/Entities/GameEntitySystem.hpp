#pragma once

#include "../../Memory.hpp"

#include "../Padding.hpp"

#include "EntityIdentity.hpp"

#include "BaseEntity.hpp"
#include "EntityInstance.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>

template <typename T>
	requires std::is_base_of_v<BaseEntity, T>
struct EntityRange {
	EntityIdentity* first;
	explicit EntityRange(EntityIdentity* first)
		: first(first)
	{
	}

	class EntityIterator {
		bool valid(EntityIdentity* identity)
		{
			return identity->instance
				&& (std::same_as<BaseEntity, T>
					|| static_cast<BaseEntity*>(identity->instance)->getSchemaType() == T::classInfo());
		}

		EntityIdentity* curr;

	public:
		explicit EntityIterator(EntityIdentity* first)
			: curr(first)
		{
			while (curr && !valid(curr)) {
				curr = curr->next();
			}
		}

		T* operator*()
		{
			return curr ? static_cast<T*>(curr->instance) : nullptr;
		}

		EntityIterator& operator++()
		{
			while (true) {
				curr = curr->next();

				if (!curr)
					break;

				if (!valid(curr))
					continue;

				break;
			}

			return *this;
		}

		bool operator==(const EntityIterator& other) const
		{
			return curr == other.curr;
		}
	};

	EntityIterator begin()
	{
		return EntityIterator{ first };
	}

	EntityIterator end()
	{
		return EntityIterator{ nullptr };
	}
};

class GameEntitySystem {
public:
	static constexpr std::size_t ENTITY_SERIAL_BITS_COUNT = 15;
	static constexpr std::uint32_t ENTITY_LIST_INDEX_MASK = (1 << ENTITY_SERIAL_BITS_COUNT) - 1;
	static constexpr std::uint32_t INVALID_ENTITY_HANDLE = 0xFFFFFFFF;

	static void resolve_signatures();
	static GameEntitySystem* the();

	constexpr EntityIdentity** entity_buckets()
	{
		// Offset obtained from get_entity_by_index
		return reinterpret_cast<EntityIdentity**>(reinterpret_cast<std::byte*>(this) + 0x10);
	}

	BaseEntity* get_entity_by_index(unsigned int index);

	// shoutout cl_showents
	OFFSET(EntityIdentity*, first_networked_entity, 0x210);

	EntityRange<BaseEntity> entities()
	{
		return EntityRange<BaseEntity>{ first_networked_entity() };
	}

	template <typename T>
	EntityRange<T> entities_of_type()
	{
		return EntityRange<T>{ first_networked_entity() };
	}
};
