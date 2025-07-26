#include "CSPlayerWeaponServices.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "glm/ext/vector_float3.hpp"

#include "../../../Memory.hpp"

glm::vec3 CSPlayerWeaponServices::get_interpolated_shoot_position(int tick, float fraction)
{
	static auto* get_interpolated_shoot_position
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"%s: GetInterpolatedShootPosition", false>(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
			  .expect<glm::vec3 (*)(CSPlayerWeaponServices*, int, float)>("Couldn't find GetInterpolatedShootPosition");

	return get_interpolated_shoot_position(this, tick, fraction);
}
