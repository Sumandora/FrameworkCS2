#pragma once

#include "../Padding.hpp"
#include "../VirtualMethod.hpp"

#include "BaseEntity.hpp"
#include "EntityIdentity.hpp"
#include "EntityInstance.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <utility>

template <typename T>
	requires std::is_base_of_v<BaseEntity, T>
struct EntityRange {
	EntityIdentity* first;
	std::shared_lock<std::shared_mutex> lock;

	explicit EntityRange(EntityIdentity* first, std::shared_lock<std::shared_mutex> lock)
		: first(first)
		, lock(std::move(lock))
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

	static std::shared_mutex entity_list_lock;

	static void resolve_signatures();
	static GameEntitySystem* the();

	constexpr EntityIdentity** entity_buckets()
	{
		// Offset obtained from get_entity_by_index
		return reinterpret_cast<EntityIdentity**>(reinterpret_cast<std::byte*>(this) + 0x10);
	}

	static std::shared_lock<std::shared_mutex> shared_lock()
	{
		return std::shared_lock{ entity_list_lock };
	}

	static std::unique_lock<std::shared_mutex> unique_lock()
	{
		return std::unique_lock{ entity_list_lock };
	}

	BaseEntity* get_entity_by_index(unsigned int index);

	// shoutout cl_showents
	OFFSET(EntityIdentity*, first_networked_entity, 0x210);

	EntityRange<BaseEntity> entities()
	{
		return EntityRange<BaseEntity>{ first_networked_entity(), std::shared_lock{ entity_list_lock } };
	}

	template <typename T>
	EntityRange<T> entities_of_type()
	{
		return EntityRange<T>{ first_networked_entity(), std::shared_lock{ entity_list_lock } };
	}

	VIRTUAL_METHOD(16, on_add_entity, void, (EntityInstance * entity, std::uint32_t handle), (this, entity, handle));
	VIRTUAL_METHOD(17, on_remove_entity, void, (EntityInstance * entity, std::uint32_t handle), (this, entity, handle));
};
