#pragma once

#include "../Padding.hpp"
#include "glm/ext/vector_float3.hpp"

// Very much a TODO

struct ViewAngles {
	PADDING(0x18);
	glm::vec3 ang_value;

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	operator glm::vec3&() {
		return ang_value;
	}
};

struct BaseCmd {
	PADDING(0x18);
	
	PADDING(0x18); // subtick moves
	void* crc;
	void* buttons;
	ViewAngles* viewangles;
}; 

struct CSGOUserCmd {
	PADDING(0xC);

	PADDING(0x18); // input history
	BaseCmd* base_cmd;
};

struct UserCmd {
	void* vtable;

	PADDING(0x10);

	CSGOUserCmd csgo_usercmd;
};
