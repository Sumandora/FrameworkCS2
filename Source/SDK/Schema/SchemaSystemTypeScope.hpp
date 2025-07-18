#pragma once

#include "../VirtualMethod.hpp"

#include "../../Utils/Logging.hpp"

struct SchemaClassInfo;

struct SchemaSystemTypeScope {
private:
	VIRTUAL_METHOD(2, find_declared_class_, SchemaClassInfo*, (const char* name), (this, name));

public:
	// TODO move to source file
	SchemaClassInfo* find_declared_class(const char* name)
	{
		SchemaClassInfo* info = find_declared_class_(name);
		if (!info) {
			Logging::error("Failed to find class '{}'", name);
		}
		return info;
	}
};
