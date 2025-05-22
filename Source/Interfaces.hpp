#pragma once

struct SchemaSystem;
struct EngineCvar;
struct EngineToClient;

namespace Interfaces {
	inline void* source2Client;
	inline SchemaSystem* schemaSystem;
	inline EngineCvar* engineCvar;
	inline EngineToClient* engine;

	void get_interfaces();

	void* uncover_create_function(void* create_func);
}
