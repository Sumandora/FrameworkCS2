#include "GameTrace.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../Memory.hpp"

static void (*init_game_trace)(GameTrace*) = nullptr;

void GameTrace::resolve_signatures()
{
	init_game_trace
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 41 0F B7 47 ? 48 89 DE">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(init_game_trace), init_game_trace);
}

GameTrace GameTrace::initialized()
{
	GameTrace trace;
	RetAddrSpoofer::invoke(init_game_trace, &trace);
	return trace;
}
