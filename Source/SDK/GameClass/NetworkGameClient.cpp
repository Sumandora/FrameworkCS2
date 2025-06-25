#include "NetworkGameClient.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "../../Memory.hpp"
#include "SignatureScanner/PatternSignature.hpp"

void NetworkGameClient::client_side_predict(int stage)
{
	// TODO use the convenient string.
	static auto* func = BCRL::signature(
		Memory::mem_mgr,
		SignatureScanner::PatternSignature::for_array_of_bytes<"80 BF ? ? ? ? 00 74 ? 55 48 89 E5 41 57">(),
		BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libengine2.so"))
	.expect<void(*)(NetworkGameClient*, int)>("Couldn't find NetworkGameClient::ClientSidePredict");

	func(this, stage);
}
