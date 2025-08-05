#pragma once

#include "../VirtualMethod.hpp"

#include "ParticleSnapshot.hpp"

#include <cstdint>

class ParticleSystemMgr {
public:
	// interesting parameter order for sure...
	STATIC_VIRTUAL_METHOD(41, create_particle_snapshot, void,
		(ParticleSnapshot * *snapshot, std::uint64_t* unk),
		(this, snapshot, this, unk));

	VIRTUAL_METHOD(42, show_particle, void,
		(ParticleSnapshot * *snapshot, int count, void* data),
		(this, snapshot, count, data));
};


