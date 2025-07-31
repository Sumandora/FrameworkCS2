#include "GameTrace.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../Memory.hpp"
#include "SignatureScanner/XRefSignature.hpp"

static void (*init_game_trace)(GameTrace*) = nullptr;

void GameTrace::resolve_signatures()
{
	init_game_trace
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"invalid_hitbox">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
			  .BCRL_EXPECT(decltype(init_game_trace), init_game_trace);
}

GameTrace GameTrace::initialized()
{
	GameTrace trace;
	RetAddrSpoofer::invoke(init_game_trace, &trace);
	return trace;
}
