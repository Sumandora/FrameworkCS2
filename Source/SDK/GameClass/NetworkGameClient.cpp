#include "NetworkGameClient.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "../../Memory.hpp"

void NetworkGameClient::client_side_predict(int stage)
{
	static auto* func
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"CNetworkGameClient::ClientSidePredict">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libengine2.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libengine2.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"80 bf 08 01 00 00 00">())
			  .expect<void (*)(NetworkGameClient*, int)>("Couldn't find NetworkGameClient::ClientSidePredict");

	func(this, stage);
}
