#include "GameHook.hpp"

#include "ldisasm.h"

#include "../Interfaces.hpp"
#include "../Memory.hpp"

#include "BCRL.hpp"

namespace GameHook {
	void hook()
	{
		FrameStageNotify::hook = new GameHook(BCRL::Session::arrayPointer(Interfaces::source2Client, 31).expect("Couldn't find FrameStageNotify"), reinterpret_cast<void*>(FrameStageNotify::hookFunc));
		ShouldShowCrosshair::hook = new GameHook(Memory::shouldShowCrosshair, reinterpret_cast<void*>(ShouldShowCrosshair::hookFunc));
	}

	void unhook()
	{
		delete ShouldShowCrosshair::hook;
		delete FrameStageNotify::hook;
	}

	GameHook::GameHook(void* original, void* hook)
	{
		size_t len = 0;
		while (len <= DetourHooking::minLength) {
			len += ldisasm(reinterpret_cast<char*>(original) + len, true);
		}
		backingHook = new DetourHooking::Hook(original, hook, len);
		proxy = backingHook->getTrampoline();
		backingHook->enable();
	}

	GameHook::~GameHook()
	{
		backingHook->disable();
		delete backingHook;
	}
}