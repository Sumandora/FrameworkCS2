#include "GameTrace.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../Memory.hpp"

GameTrace GameTrace::initialized()
{
	GameTrace trace;

	static auto* init_game_trace = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 48 8D 05 ? ? ? ? 48 8D 8D ? ? ? ? 48 C7 85 ? ? ? ? 00 00 00 00">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
									   .add(1)
									   .relative_to_absolute()
									   .expect<void (*)(GameTrace*)>("Couldn't find InitGameTrace");

	RetAddrSpoofer::invoke(init_game_trace, &trace);

	return trace;
}
