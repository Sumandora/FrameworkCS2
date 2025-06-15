#include "Protobuf.hpp"

#include "google/protobuf/port_def.inc"

#include "Logging.hpp"

#include "../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include <cstdint>
#include <expected>

void Protobuf::verify_version()
{
	std::expected<std::uint32_t*, BCRL::FinalizationError> version
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"This program was compiled against version ">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .sub(41)
			  .finalize<std::uint32_t*>();

	if (version.has_value()) {
		std::uint32_t version_in_game = *version.value();
		if (version_in_game != PROTOBUF_VERSION)
			Logging::error("Protobuf version mismatch. Expected {} but got {}", PROTOBUF_VERSION, version_in_game);
	} else
		Logging::error("Failed to verify protobuf version");
}
