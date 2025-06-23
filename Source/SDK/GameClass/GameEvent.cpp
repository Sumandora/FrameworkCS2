#include "GameEvent.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "../../Memory.hpp"

const char* GameEvent::get_string(const char* key_name) const
{
	// TODO I think this can be rebuilt easily, but I just don't feel like it right now.
	static auto* get_string = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 41 56 4C 8D B5 ? ? ? ? 41 55 49 89 D5 41 54 49 89 FC 53 48 89 F3 48 81 EC A8 00 00 00 48 8B 07 4C 8B 78">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
								  .expect<const char* (*)(const void*, const char*)>("Couldn't find GameEvent::get_string");

	// basically calls vfunc 11 but instead of const char*, it uses UtlBufferString.
	return get_string(this, key_name);
}
