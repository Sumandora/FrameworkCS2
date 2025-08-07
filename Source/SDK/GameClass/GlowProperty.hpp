#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../Entities/BaseEntity.hpp"

#include "../Padding.hpp"

struct GlowProperty {
	CLASS_INFO("libclient.so", "CGlowProperty");

	// In the ApplyGlow function, if the glow is enabled (glowing == 1)
	// then a vfunc is called on the entity, there the offset can be obtained.
	OFFSET(BaseEntity*, entity, 0x18);
};
