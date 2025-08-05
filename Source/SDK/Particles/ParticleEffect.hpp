#pragma once

#include <cstring>

struct ParticleEffect {
	const char* name;
	int unk = 8;
	char pad[0x3c]{};

	explicit ParticleEffect(const char* name)
		: name(name)
	{
	}
};

static_assert(sizeof(ParticleEffect) == 0x48);
