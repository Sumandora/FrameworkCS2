#include "Hooks.hpp"

#include "../Memory.hpp"

#include "BCRL.hpp"

#include "DetourHooking.hpp"
using namespace DetourHooking;

#include "RetAddrSpoofer.hpp"

#include "../SDK/GameClass/GameEvent.hpp"

static Hook* fireEventHook;

extern std::unordered_map<std::string, std::size_t> eventCounter; // Defined in Main.cpp as POC

static void* fireEventHookFunc(void* gameEventManager, GameEvent* event, bool rdx, bool rcx) {
	eventCounter[event->GetName()]++;
	return RetAddrSpoofer::invoke<void*>(fireEventHook->getTrampoline(), gameEventManager, event, rdx, rcx);
}

void Hooks::Create()
{
	fireEventHook = new Hook(Memory::fireEvent, reinterpret_cast<void*>(fireEventHookFunc), CalculateHookLength(Memory::fireEvent));

	fireEventHook->enable();
}

void Hooks::Destroy()
{
	delete fireEventHook;
}

std::size_t Hooks::CalculateHookLength(void* address)
{
	char* cPointer = reinterpret_cast<char*>(address);
	auto ptr = BCRL::Session::pointer(cPointer).repeater([cPointer](BCRL::SafePointer& safePointer) {
												   safePointer = safePointer.nextInstruction();
												   return reinterpret_cast<char*>(safePointer.getPointer()) - cPointer < minLength;
											   }).getPointer();

	return reinterpret_cast<char*>(ptr.value()) - cPointer;
}