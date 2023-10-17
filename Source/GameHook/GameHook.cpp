#include "GameHook.hpp"

#include "ldisasm.h"

#include "../Interfaces.hpp"
#include "../Utils/VMT.hpp"

namespace GameHook {
	void hook()
	{
		FrameStageNotify::hook = new GameHook(vtable(Interfaces::source2Client)[31], reinterpret_cast<void*>(FrameStageNotify::hookFunc));
	}

	void unhook()
	{
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