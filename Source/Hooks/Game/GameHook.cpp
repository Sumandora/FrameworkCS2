#include "GameHook.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "BCRL/Session.hpp"
#include "DetourHooking.hpp"
#include "LengthDisassembler/LengthDisassembler.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace Hooks::Game {
	void create()
	{
		FrameStageNotify::hook.emplace(BCRL::pointer_array(Memory::mem_mgr, Interfaces::source2Client, 36).expect<void*>("Couldn't find FrameStageNotify"), reinterpret_cast<void*>(FrameStageNotify::hookFunc));
		ShouldShowCrosshair::hook.emplace(Memory::shouldShowCrosshair, reinterpret_cast<void*>(ShouldShowCrosshair::hookFunc));
		FireEvent::hook.emplace(Memory::fireEvent, reinterpret_cast<void*>(FireEvent::hookFunc));
	}

	void destroy()
	{
		FireEvent::hook.reset();
		ShouldShowCrosshair::hook.reset();
		FrameStageNotify::hook.reset();
	}

	static std::size_t get_stolen_bytes_count(void* instruction)
	{
		size_t len = 0;
		while (len <= DetourHooking::MIN_LENGTH) {
			auto insn = LengthDisassembler::disassemble(reinterpret_cast<std::byte*>(instruction) + len);
			len += insn.value().length;
		}
		return len;
	}

	GameHook::GameHook(void* original, void* hook)
		: backingHook{ Memory::emalloc, original, hook, get_stolen_bytes_count(original) }
	{
		backingHook.enable();
	}

	GameHook::~GameHook()
	{
		backingHook.disable();
	}

	void* GameHook::get_proxy() const noexcept
	{
		return reinterpret_cast<void*>(backingHook.get_trampoline());
	}
}
