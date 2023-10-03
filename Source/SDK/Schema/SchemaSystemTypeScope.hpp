#pragma once

#include "../VirtualMethod.hpp"

struct SchemaClassInfo;

class SchemaSystemTypeScope {
public:
	VIRTUAL_METHOD(2, FindDeclaredClass, SchemaClassInfo*, (const char* name), (this, name));
};