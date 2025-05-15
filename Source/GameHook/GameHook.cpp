#include "GameHook.hpp"

#include "../Interfaces.hpp"
#include "../Memory.hpp"

#include "BCRL/Session.hpp"
#include "DetourHooking.hpp"
#include "LengthDisassembler/LengthDisassembler.hpp"

#include <cstddef>
#include <cstdint>

namespace GameHook {
	void hook()
	{
		FrameStageNotify::hook = new GameHook(BCRL::pointer_array(Memory::mem_mgr, (std::uintptr_t)Interfaces::source2Client, 33).expect<void*>("Couldn't find FrameStageNotify"), reinterpret_cast<void*>(FrameStageNotify::hookFunc));
		ShouldShowCrosshair::hook = new GameHook(Memory::shouldShowCrosshair, reinterpret_cast<void*>(ShouldShowCrosshair::hookFunc));
		FireEvent::hook = new GameHook(Memory::fireEvent, reinterpret_cast<void*>(FireEvent::hookFunc));
	}

	void unhook()
	{
		delete FireEvent::hook;
		delete ShouldShowCrosshair::hook;
		delete FrameStageNotify::hook;
	}

	GameHook::GameHook(void* original, void* hook)
	{
		size_t len = 0;
		while (len <= DetourHooking::MIN_LENGTH) {
			auto insn = LengthDisassembler::disassemble(reinterpret_cast<std::byte*>(original) + len);
			len += insn.value().length;
		}
		backingHook = new DetourHooking::Hook<true, decltype(Memory::mem_mgr)>(Memory::emalloc, original, hook, len);
		proxy = (void*)backingHook->get_trampoline();
		backingHook->enable();
	}

	GameHook::~GameHook()
	{
		backingHook->disable();
		delete backingHook;
	}
}
