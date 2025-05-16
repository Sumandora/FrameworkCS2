#include "Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "Interfaces.hpp"

#include "SDK/Entities/GameEntitySystem.hpp"
#include "SDK/GameClass/ViewRender.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"
#include <cstdint>

const void* RetAddrSpoofer::leaveRet;

static void FindEntitySystem()
{
	// To the valve employee, who wrote this function: Kuss auf Nuss!
	auto /*CPrediction::*/ reinitPredictables = BCRL::signature(Memory::mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"CL:  reinitialized %i predictable entities\n">(), BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
													.find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
													.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 83 ec ? 4c 8d 35">())
													.add(7);

	printf("ReinitPredictables at %p\n", reinitPredictables.expect("Can't find ReinitPredictables"));

	Memory::EntitySystem::gameEntitySystem = reinitPredictables.clone().relative_to_absolute().expect<GameEntitySystem**>("Couldn't find GameEntitySystem");
	printf("Found GameEntitySystem at: %p\n", Memory::EntitySystem::gameEntitySystem);
	reinitPredictables = reinitPredictables.add(4).next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"49 8b 3e e8">()).add(4);
	Memory::EntitySystem::getHighestEntityIndex = reinitPredictables.clone().relative_to_absolute().expect<void*>("Couldn't find getHighestEntityIndex");
	printf("Found getHighestEntityIndex at: %p\n", Memory::EntitySystem::getHighestEntityIndex);
	reinitPredictables = reinitPredictables.add(4).next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"3e 44 89 fe e8">()).add(5);
	Memory::EntitySystem::getBaseEntity = reinitPredictables.clone().relative_to_absolute().expect<void*>("Couldn't find getBaseEntity");
	printf("Found getBaseEntity at: %p\n", Memory::EntitySystem::getBaseEntity);
}

void Memory::Create()
{
	// Set the address for the return address spoofer
	RetAddrSpoofer::leaveRet = BCRL::pointer_array(mem_mgr, (std::uintptr_t)Interfaces::source2Client, 0) // random code piece
								   .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"c9 c3">(), BCRL::everything(mem_mgr).thats_readable().thats_executable())
								   .expect<void*>("Couldn't find a *leave; ret* pattern");

	viewRender = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"restart_in_untrusted">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
					 .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
					 .add(4)
					 .repeater([](auto& safePointer) {
						 static std::size_t nthCall = 0;

						 BCRL::SafePointer nextInstruction = safePointer.clone().next_instruction();

						 if (nextInstruction.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
							 nthCall++;

						 if (nthCall >= 3) {
							 // third call instruction is the ViewRender Constructor
							 // The first argument is the pointer to g_ViewRender
							 return false;
						 }

						 safePointer = nextInstruction;
						 return true;
					 })
					 .add(3)
					 .relative_to_absolute()
					 .expect<ViewRender*>("Couldn't find ViewRender structure");

	printf("Found ViewRender at: %p\n", viewRender);

	//  CRenderGameSystem::GetMatricesForView
	//            (_g_pRenderGameSystem,(CViewSetup *)(CFrustum *)(this + 0x10),(VMatrix *)&g_WorldToView,
	//             (VMatrix *)&g_ViewToProjection,(VMatrix *)&_g_WorldToProjection,
	//             (VMatrix *)&g_WorldToScreen);
	worldToProjectionMatrix = BCRL::pointer_array(mem_mgr, (std::uintptr_t)viewRender, ViewRender::onRenderStartIdx)
								  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"4c 8d 05">())
								  .add(3)
								  .relative_to_absolute()
								  .expect<VMatrix*>("Couldn't find WorldToProjection matrix");

	printf("Found WorldToProjection matrix at: %p\n", worldToProjectionMatrix);

	FindEntitySystem();

	shouldShowCrosshair = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"weapon_reticle_knife_show">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
							  .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
							  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 3d">())
							  .add(3)
							  .relative_to_absolute() // This is a ConVar instance (not the ConVar type in the SDK)
							  .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
							  .add(4)
							  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"be ff ff ff ff">()); })
							  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
							  .expect<void*>("Couldn't find shouldShowCrosshair");

	getLocalPlayer = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"cl_sim_grenade_trajectory">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
						 .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
						 .sub(11)
						 .relative_to_absolute()
						 .repeater([](auto& ptr) {
							 ptr.next_instruction();
							 static int i = 0;
							 if (ptr.does_match(
									 SignatureScanner::PatternSignature::for_array_of_bytes<
										 "e8">()))
								 i++;
							 return i != 2;
						 })
						 .add(1)
						 .relative_to_absolute()
						 .expect<void*>("Couldnt find getLocalPlayer");

	fireEvent = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"FireEvent: event '%s' not registered.\n">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
					.find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
					.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 8d 87 ? ? ? ? 48 89 e5">()) // What is the lea in the middle of the prologue?
					.expect<void*>("Couldn't find FireEvent");

	csgo_input = BCRL::signature(mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"cl_interp_ratio">(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
					 .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(mem_mgr).thats_readable().with_name("libclient.so"))
					 .sub(27)
					 .filter([](const auto& ptr) {
						 return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 35">());
					 })
					 .add(3)
					 .relative_to_absolute()
					 .expect<void*>("Couldn't find CCSGOInput");
}
