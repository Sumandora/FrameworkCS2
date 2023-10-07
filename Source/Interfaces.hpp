#pragma once

struct SchemaSystem;
struct EngineCvar;

namespace Interfaces {
	inline void* source2Client;
	inline SchemaSystem* schemaSystem;
	inline EngineCvar* engineCvar;

	void getInterfaces();

	void* uncoverCreateFunction(void* createFunc);
}
