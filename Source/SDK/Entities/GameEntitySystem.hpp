#pragma once

#include "../../Memory.hpp"

#include "../Padding.hpp"

#include "EntityIdentity.hpp"

#include "BaseEntity.hpp"

#include <concepts>
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
	static void resolve_signatures();
	static GameEntitySystem* the();

	BaseEntity* get_entity_by_index(int index);

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
