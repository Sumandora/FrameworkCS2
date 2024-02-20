#pragma once

#include <cstdint>

#include "../../Memory.hpp"
#include "../Entities/GameEntitySystem.hpp"

template <typename E>
struct Handle {
	explicit Handle(std::uint32_t index)
		: index(index)
	{
	}
	static Handle invalid() { return Handle{ INVALID_INDEX }; }

private:
	std::uint32_t index;
	constexpr static std::uint32_t INVALID_INDEX = 0xffffffff;

public:
	constexpr static std::uint32_t ENTRY_MASK = 0x7fff;

	[[nodiscard]] bool isValid() const { return index != INVALID_INDEX; }

	[[nodiscard]] std::uint32_t getIndex() const { return index & ENTRY_MASK; }

	bool operator==(const Handle& other) const { return getIndex() == other.getIndex(); }

	[[nodiscard]] E* get() const
	{
		if (!isValid()) {
			return nullptr;
		}
		auto* entity = Memory::gameEntitySystem->getBaseEntity(getIndex());
		return static_cast<E*>(entity);
	}

	[[nodiscard]] E* operator->() const { return get(); }

	template <class O>
	Handle<O> as() const
	{
		return Handle<O>{ index };
	}
};
