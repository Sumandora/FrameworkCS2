#include "EngineTrace.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include "glm/ext/vector_float3.hpp"

#include "../../Memory.hpp"

static EngineTrace** engine_trace = nullptr;
static bool (*trace_shape)(EngineTrace* thisptr, Ray* ray, const glm::vec3* from, const glm::vec3* to, TraceFilter* filter, GameTrace* trace) = nullptr;

void EngineTrace::resolve_signatures()
{
	engine_trace
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8D 05 ? ? ? ? 4D 89 E1 66 0F EF C0 4C 8B 85">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .BCRL_EXPECT(EngineTrace**, engine_trace);

	::trace_shape
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"Physics/TraceShape (Client)">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
			  .BCRL_EXPECT(decltype(::trace_shape), ::trace_shape);
}

EngineTrace* EngineTrace::the()
{
	return *engine_trace;
}

bool EngineTrace::trace_shape(Ray* ray, const glm::vec3& from, const glm::vec3& to, TraceFilter* filter, GameTrace* trace)
{
	return RetAddrSpoofer::invoke(::trace_shape, this, ray, &from, &to, filter, trace);
}
