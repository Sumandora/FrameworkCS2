#pragma once

#include "../VirtualMethod.hpp"

#include <cstdint>

struct SchemaSystemTypeScope;
struct SchemaClassInfo;

struct SchemaSystem {
	VIRTUAL_METHOD(13, FindTypeScopeForModule, SchemaSystemTypeScope*, (const char* name), (this, name, 0));

	static std::int32_t findOffset(SchemaClassInfo* class_info, const char* field_name);
};
