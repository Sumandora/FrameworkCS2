#pragma once

#include "SDK/Schema/SchemaSystem.hpp"

namespace Interfaces {
	inline void* source2Client;
	inline SchemaSystem* schemaSystem;

	void getInterfaces();

	void* uncoverCreateFunction(void* createFunc);
}
