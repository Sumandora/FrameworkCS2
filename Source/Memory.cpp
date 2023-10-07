#include "Memory.hpp"

#include "BCRL.hpp"

#include "Interfaces.hpp"

#include "SDK/GameClass/ViewRender.hpp"

void* RetAddrSpoofer::leaveRet;

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
			.expect("Couldn't find view render structure"));

	//  CRenderGameSystem::GetMatricesForView
	//            (_g_pRenderGameSystem,(CViewSetup *)(CFrustum *)(this + 0x10),(VMatrix *)&g_WorldToView,
	//             (VMatrix *)&g_ViewToProjection,(VMatrix *)&_g_WorldToProjection,
	//             (VMatrix *)&g_WorldToScreen);
	worldToProjectionMatrix = static_cast<VMatrix*>(
		BCRL::Session::arrayPointer(viewRender, ViewRender::onRenderStartIdx)
			.nextByteOccurrence("4c 8d 05")
			.add(3)
			.relativeToAbsolute()
			.expect("Couldn't find world to projection matrix"));

	printf("%p", worldToProjectionMatrix);
}