#pragma once

#include "../VirtualMethod.hpp"

struct SchemaSystemTypeScope;

class SchemaSystem {
public:
	VIRTUAL_METHOD(13, FindTypeScopeForModule, SchemaSystemTypeScope*, (const char* name), (this, name, 0));
};