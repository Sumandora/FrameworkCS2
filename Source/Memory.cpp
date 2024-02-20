#include "Memory.hpp"

#include "BCRL.hpp"

#include "Interfaces.hpp"
#include "SDK/Entities/GameEntitySystem.hpp"

#include "SDK/GameClass/ViewRender.hpp"

void* RetAddrSpoofer::leaveRet;

void Memory::Create()
{
	// Set the address for the return address spoofer
	RetAddrSpoofer::leaveRet = BCRL::Session::pointerArray(Interfaces::source2Client, 0) // random code piece
								   .nextByteOccurrence("c9 c3", true)
								   .expect("Couldn't find a *leave; ret* pattern");

	viewRender = BCRL::Session::module("libclient.so")
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
			.expect<ViewRender*>("Couldn't find ViewRender structure");

	printf("Found ViewRender at: %p\n", viewRender);

	//  CRenderGameSystem::GetMatricesForView
	//            (_g_pRenderGameSystem,(CViewSetup *)(CFrustum *)(this + 0x10),(VMatrix *)&g_WorldToView,
	//             (VMatrix *)&g_ViewToProjection,(VMatrix *)&_g_WorldToProjection,
	//             (VMatrix *)&g_WorldToScreen);
	worldToProjectionMatrix = BCRL::Session::pointerArray(viewRender, ViewRender::onRenderStartIdx)
			.nextByteOccurrence("4c 8d 05")
			.add(3)
			.relativeToAbsolute()
			.expect<VMatrix*>("Couldn't find WorldToProjection matrix");

	printf("Found WorldToProjection matrix at: %p\n", worldToProjectionMatrix);

	gameEntitySystem = new GameEntitySystem();

	shouldShowCrosshair = BCRL::Session::module("libclient.so")
							  .nextStringOccurrence("weapon_reticle_knife_show")
							  .findXREFs("libclient.so", true, false)
							  .nextByteOccurrence("48 8d 3d")
							  .add(3)
							  .relativeToAbsolute() // This is a ConVar instance (not the ConVar type in the SDK)
							  .findXREFs("libclient.so", true, false)
							  .add(4)
							  .filter([](const BCRL::SafePointer& ptr) { return ptr.doesMatch("be ff ff ff ff"); })
							  .prevByteOccurrence("55 48 89 e5")
							  .expect("Couldn't find shouldShowCrosshair");

	getLocalPlayer = BCRL::Session::pointer(shouldShowCrosshair)
						 .repeater([](BCRL::SafePointer& ptr) {
							 ptr = ptr.nextInstruction();
							 return !ptr.doesMatch("e8");
						 })
						 .add(1)
						 .relativeToAbsolute()
						 .expect("Couldn't find getLocalPlayer");

	fireEvent = BCRL::Session::module("libclient.so")
					.nextStringOccurrence("FireEvent: event '%s' not registered.\n")
					.findXREFs("libclient.so", true, false)
					.prevByteOccurrence("55 48 8d 87 ? ? ? ? 48 89 e5") // What is the lea in the middle of the prologue?
					.expect("Couldn't find FireEvent");
}