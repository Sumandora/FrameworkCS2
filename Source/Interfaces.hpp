#pragma once

struct SchemaSystem;

namespace Interfaces {
	inline void* source2Client;
	inline SchemaSystem* schemaSystem;

	void getInterfaces();

	void* uncoverCreateFunction(void* createFunc);
}
