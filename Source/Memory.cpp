#include "Memory.hpp"

#include "BCRL.hpp"

#include "Interfaces.hpp"

#include "SDK/GameClass/EngineCvar.hpp"
#include "SDK/GameClass/ViewRender.hpp"

void* RetAddrSpoofer::leaveRet;

static void FindEntitySystem()
{
	// To the valve employee, who wrote this function: Kuss auf Nuss!
	auto /*CPrediction::*/ reinitPredictables = BCRL::Session::module("libclient.so")
													.nextStringOccurrence("CL:  reinitialized %i predictable entities\n")
													.findXREFs("libclient.so", true, false)
													.prevByteOccurrence("48 83 ec ? 4c 8d 35")
													.add(7);

	printf("ReinitPredictables at %p\n", reinitPredictables.expect("Can't find ReinitPredictables"));

	Memory::EntitySystem::gameEntitySystem = static_cast<GameEntitySystem**>(reinitPredictables.relativeToAbsolute().expect("Couldn't find GameEntitySystem"));
	printf("Found GameEntitySystem at: %p\n", Memory::EntitySystem::gameEntitySystem);
	reinitPredictables = reinitPredictables.add(4).nextByteOccurrence("49 8b 3e e8").add(4);
	Memory::EntitySystem::getHighestEntityIndex = reinitPredictables.relativeToAbsolute().expect("Couldn't find getHighestEntityIndex");
	printf("Found getHighestEntityIndex at: %p\n", Memory::EntitySystem::getHighestEntityIndex);
	reinitPredictables = reinitPredictables.add(4).nextByteOccurrence("3e 44 89 fe e8").add(5);
	Memory::EntitySystem::getBaseEntity = reinitPredictables.relativeToAbsolute().expect("Couldn't find getBaseEntity");
	printf("Found getBaseEntity at: %p\n", Memory::EntitySystem::getBaseEntity);
}

void Memory::Create()
{
	// Set the address for the return address spoofer
	RetAddrSpoofer::leaveRet = BCRL::Session::arrayPointer(Interfaces::source2Client, 0) // random code piece
								   .nextByteOccurrence("c9 c3", true)
								   .expect("Couldn't find a *leave; ret* pattern");

	viewRender = static_cast<ViewRender*>(
		BCRL::Session::module("libclient.so")
			.nextStringOccurrence("restart_in_untrusted")
			.findXREFs("libclient.so", true, false)
			.add(4)
			.repeater([](BCRL::SafePointer& safePointer) {
				static std::size_t nthCall = 0;

				BCRL::SafePointer nextInstruction = safePointer.nextInstruction();

				if (nextInstruction.doesMatch("e8"))
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
			.relativeToAbsolute()
			.expect("Couldn't find ViewRender structure"));

	printf("Found ViewRender at: %p\n", viewRender);

	//  CRenderGameSystem::GetMatricesForView
	//            (_g_pRenderGameSystem,(CViewSetup *)(CFrustum *)(this + 0x10),(VMatrix *)&g_WorldToView,
	//             (VMatrix *)&g_ViewToProjection,(VMatrix *)&_g_WorldToProjection,
	//             (VMatrix *)&g_WorldToScreen);
	worldToProjectionMatrix = static_cast<VMatrix*>(
		BCRL::Session::arrayPointer(viewRender, ViewRender::onRenderStartIdx)
			.nextByteOccurrence("4c 8d 05")
			.add(3)
			.relativeToAbsolute()
			.expect("Couldn't find WorldToProjection matrix"));

	printf("Found WorldToProjection matrix at: %p\n", worldToProjectionMatrix);

	FindEntitySystem();

	shouldShowCrosshair = BCRL::Session::module("libclient.so")
							  .nextStringOccurrence("weapon_reticle_knife_show")
							  .findXREFs("libclient.so", true, false)
							  .nextByteOccurrence("48 8d 3d")
							  .add(3)
							  .relativeToAbsolute()  // This is a ConVar instance (not the ConVar type in the SDK)
							  .findXREFs("libclient.so", true, false)
							  .add(4)
							  .filter([](const BCRL::SafePointer& ptr) { return ptr.doesMatch("be ff ff ff ff"); })
							  .prevByteOccurrence("55 48 89 e5")
							  .expect("Couldn't find shouldShowCrosshair");

	getLocalPlayer = BCRL::Session::pointer(shouldShowCrosshair).repeater([](BCRL::SafePointer& ptr) {
																	ptr = ptr.nextInstruction();
																	return !ptr.doesMatch("e8");
																})
						 .add(1)
						 .relativeToAbsolute()
						 .expect("Couldn't find getLocalPlayer");
}