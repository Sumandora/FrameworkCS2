#include "EngineTrace.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "RetAddrSpoofer.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "glm/ext/vector_float3.hpp"

#include "../../Memory.hpp"

static EngineTrace** engine_trace;
static bool (*trace_shape)(EngineTrace* thisptr, Ray* ray, const glm::vec3* from, const glm::vec3* to, TraceFilter* filter, GameTrace* trace);

void EngineTrace::resolve_signatures()
{
	engine_trace
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8D 05 ? ? ? ? 4D 89 E1 66 0F EF C0 4C 8B 85">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .expect<EngineTrace**>("Couldn't find engine trace");

	::trace_shape
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? F3 0F 10 05 ? ? ? ? 0F 2F 45 ? 77 ? 80 7D ? 00 0F 84 ? ? ? ? F3 0F 10 4D">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(::trace_shape)>("Couldn't find trace shape");
}

EngineTrace* EngineTrace::the()
{
	if (!engine_trace)
		return nullptr;
	return *engine_trace;
}

bool EngineTrace::trace_shape(Ray* ray, const glm::vec3& from, const glm::vec3& to, TraceFilter* filter, GameTrace* trace)
{
	return RetAddrSpoofer::invoke(::trace_shape, this, ray, &from, &to, filter, trace);
}
