#include "Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "Interfaces.hpp"

#include "RetAddrSpoofer.hpp"
#include "SDK/ChatPrintf.hpp"
#include "SDK/EngineTrace/EngineTrace.hpp"
#include "SDK/Entities/CSPlayerController.hpp"
#include "SDK/Entities/GameEntitySystem.hpp"
#include "SDK/GameClass/ClientModeCSNormal.hpp"
#include "SDK/GameClass/CSGOInput.hpp"
#include "SDK/GameClass/MemAlloc.hpp"
#include "SDK/GameClass/UserCmd.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "Utils/BulletSimulation.hpp"
#include "Utils/CRC.hpp"
#include "Utils/Logging.hpp"
#include "Utils/MovementQuantization.hpp"
#include "Utils/Projection.hpp"

const void* RetAddrSpoofer::leaveRet;

static void* get_smoke_density_in_line = nullptr;

void Memory::create()
{
	// Set the address for the return address spoofer
	RetAddrSpoofer::leaveRet = BCRL::pointer_array(mem_mgr, Interfaces::source2_client, 0) // random code piece
								   .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"c9 c3">(), BCRL::everything(mem_mgr).thats_readable().thats_executable())
								   .expect<void*>("Couldn't find a *leave; ret* pattern");

	Projection::resolve_signatures();

	// Make classes run their searches
	GameEntitySystem::the();

	shouldShowCrosshair = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"weapon_reticle_knife_show">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
							  .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
							  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"4c 8d 3d ? ? ? ? b9">())
							  .add(3)
							  .relative_to_absolute() // This is a ConVar instance (not the ConVar type in the SDK)
							  .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
							  .add(4)
							  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"be ff ff ff ff">()); })
							  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
							  .expect<void*>("Couldn't find shouldShowCrosshair");

	local_player_controller = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"cl_sim_grenade_trajectory">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
								  .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
								  .sub(10)
								  .relative_to_absolute()
								  .repeater([](auto& ptr) {
									  ptr.next_instruction();
									  static int i = 0;
									  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
										  i++;
									  return i != 2;
								  })
								  .add(1)
								  .relative_to_absolute()
								  .repeater([](auto& ptr) {
									  ptr.next_instruction();
									  return !ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">());
								  })
								  .add(1)
								  .relative_to_absolute()
								  .repeater([](auto& ptr) {
									  ptr.next_instruction();
									  return !ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 05">());
								  })
								  .add(3)
								  .relative_to_absolute()
								  .expect<CSPlayerController**>("Couldn't find local_player_controller");

	Logging::info("Found local_player_controller at: {}", local_player_controller);

	fireEvent = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"FireEvent: event '%s' not registered.\n">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
					.find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
					.sub(3)
					.filter([](const auto& ptr) {
						return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 3d">());
					})
					.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 8d 87 ? ? ? ? 48 89 e5">()) // What is the lea in the middle of the prologue?
					.expect<void*>("Couldn't find FireEvent");

	csgo_input = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"cl_interpolate">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
					 .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
					 .sub(7)
					 .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"66 0f ef c9">()); })
					 .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"4c 8d 3d">())
					 .add(3)
					 .relative_to_absolute()
					 .expect<CSGOInput*>("Couldn't find CCSGOInput");

	get_fun_loading = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"#LoadingProgress_CSFunLoading%d">(), BCRL::everything(mem_mgr).thats_readable().thats_not_executable().with_name("libclient.so"))
						  .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
						  .sub(3)
						  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 35">()); })
						  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
						  .expect<void*>("Couldn't find get_fun_loading");

	MemAlloc::the(); // Acquire the allocator now.
	CRC::resolve_signatures();
	UserCmd::resolve_signatures();

	globals = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 05 ? ? ? ? 48 8b 00 8b 50 ? 31 c0 e8 ? ? ? ? 48 8d 95">(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
				  .add(3)
				  .relative_to_absolute()
				  .expect<GlobalVars**>("Couldn't find global vars");

	EngineTrace::resolve_signatures();
	BulletSimulation::resolve_signatures();

	client_mode_cs_normal
		= BCRL::signature(
			mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"cl_scoreboard_mouse_enable_binding">(),
			BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(4)
			  .filter([](const auto& ptr) {
				  return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"0f 11 4d 97">());
			  })
			  // TODO From before update, but still valid: fix in BCRL: signatures should not count if you are matching right now.
			  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 35">())
			  .add(3)
			  .relative_to_absolute()
			  .expect<ClientModeCSNormal*>("Couldn't find ClientModeCSNormal");

	MovementQuantization::init();
	CSPlayerController::resolve_signatures();
	ChatPrintf::resolve_signatures();

	::get_smoke_density_in_line
		= (void*)BCRL::signature(
			mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"*ZN11SmokeVolume21GetSmokeDensityInLineERK5Vec3DIfES3_PS1_EUlS3_jE_">(),
			BCRL::everything(mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(
				  SignatureScanner::XRefTypes::absolute(),
				  BCRL::everything(mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .sub(8)
			  .find_xrefs(
				  SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
			  .find_xrefs(
				  SignatureScanner::XRefTypes::absolute(),
				  BCRL::everything(mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(
				  SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
			  // TODO Fix in BCRL: remove duplicates. (probably should use std::set instead of vector)
			  // .expect<void*>("Couldn't find GetSmokeDensityInLine");
			  .peek()
			  .front()
			  .get_pointer();
}

float Memory::get_smoke_density_in_line(const glm::vec3& from, const glm::vec3& to)
{
	// There is also GetSmokeDensityLOS, but that one does some weird remap where the return value is remapped from [0.0;0.2] to [0.0;1.0]
	// This function seems to give the rawest result.
	//
	// TODO This function does not respect holes.
	return RetAddrSpoofer::invoke<float>(::get_smoke_density_in_line, &from, &to, nullptr);
}
