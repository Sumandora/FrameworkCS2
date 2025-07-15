#pragma once

#include "../Padding.hpp"

#include <cstddef>

struct EconItemDefinition {
	PADDING(0x200);

public:
	const char* name;

	// TODO convert to struct with paddings
	OFFSET(const char*, internal_name, 0x70);
	OFFSET(const char*, type_name, 0x80);
	OFFSET(const char*, description, 0x90);
	OFFSET(const char*, inventory_image_path, 0xa8);
	OFFSET(const char*, model_path, 0xD8);
};

static_assert(offsetof(EconItemDefinition, name) == 0x200);
