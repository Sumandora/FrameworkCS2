#pragma once

#include "../VirtualMethod.hpp"

struct SchemaClassInfo;

struct SchemaSystemTypeScope {
	VIRTUAL_METHOD(2, FindDeclaredClass, SchemaClassInfo*, (const char* name), (this, name));
};