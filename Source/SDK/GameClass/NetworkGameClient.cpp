#include "NetworkGameClient.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "../../Memory.hpp"

#include "../../Utils/Logging.hpp"

static void (*client_side_predict)(NetworkGameClient*, PredictionStage) = nullptr;

void NetworkGameClient::resolve_signatures()
{
	::client_side_predict
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"CNetworkGameClient::ClientSidePredict">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libengine2.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libengine2.so"))
			  // This skips over the following check, I don't think it's too important, so who cares...
			  // if (*(char *)(param_1 + 0xf8) == '\0') {
			  //   return;
			  // }
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
			  .expect<decltype(::client_side_predict)>("Couldn't find NetworkGameClient::ClientSidePredict");

	Logging::info("Found CNetworkGameClient::ClientSidePredict at {}", ::client_side_predict);
}

void NetworkGameClient::client_side_predict(PredictionStage stage)
{
	::client_side_predict(this, stage);
}
