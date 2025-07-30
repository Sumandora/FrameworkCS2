#include "TraceFilter.hpp"

#include "../EntityHandle.hpp"

#include "../Entities/BaseEntity.hpp"
#include "../GameClass/CollisionProperty.hpp"

#include <cstddef>
#include <cstdint>

TraceFilter::TraceFilter(std::uint64_t mask)
	: mask(mask)
	, unk1(0)
	, unk2(0)
	, handles{ EntityHandle<BaseEntity>::invalid(), EntityHandle<BaseEntity>::invalid() }
	, owners{ EntityHandle<BaseEntity>::invalid(), EntityHandle<BaseEntity>::invalid() }
	, collisions{ 0, 0 }
	, unk3(0x30f)
	, unk4(0x49)
	, unk5(0)
	, unk6(0)
{
}

void TraceFilter::add_skip(BaseEntity* e)
{
	const std::size_t idx = handles[0].has_entity() ? 1 : 0;

	handles[idx] = e->get_handle();
	owners[idx] = e->owner_entity();
	collisions[idx] = e->collision() ? e->collision()->collision_mask() : 0;
}
