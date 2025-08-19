#pragma once

#include "../EntityHandle.hpp"

#include <array>
#include <cstdint>

struct BaseEntity;

struct TraceFilter {
	std::uint64_t mask;
	std::uint64_t unk1;
	std::uint64_t unk2;
	std::array<EntityHandle<BaseEntity>, 2> handles;
	std::array<EntityHandle<BaseEntity>, 2> owners;
	std::array<std::uint16_t, 2> collisions;
	std::uint16_t unk3;
	std::uint8_t unk4;
	std::uint8_t unk5;
	std::uint8_t unk6; // removed i think, TODO

	static constexpr std::uint64_t MASK1 = 0x1c300b; // Penetration mask, no clue what it means.

	explicit TraceFilter(std::uint64_t mask);

	void add_skip(BaseEntity* entity);

	virtual ~TraceFilter() = default;
	virtual std::uint64_t unk() { return 1; }
};

static_assert(sizeof(TraceFilter) == 0x40);
