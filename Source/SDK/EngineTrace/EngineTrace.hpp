#pragma once

#include "GameTrace.hpp"
#include "TraceFilter.hpp"
#include "glm/ext/vector_float3.hpp"

struct Ray;
struct TraceFilter;
struct GameTrace;

class EngineTrace {
public:
	static void resolve_signatures();
	static EngineTrace* the();

	bool trace_shape(Ray* ray, const glm::vec3& from, const glm::vec3& to, TraceFilter* filter, GameTrace* trace);
};
