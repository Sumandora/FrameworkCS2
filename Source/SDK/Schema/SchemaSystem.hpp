#pragma once

#include "../VirtualMethod.hpp"

struct SchemaSystemTypeScope;

struct SchemaSystem {
	VIRTUAL_METHOD(13, FindTypeScopeForModule, SchemaSystemTypeScope*, (const char* name), (this, name, 0));
};