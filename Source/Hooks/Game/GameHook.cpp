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
		FrameStageNotify::hook.emplace(
			Memory::emalloc,
			BCRL::pointer_array(Memory::mem_mgr, Interfaces::source2Client, 36)
				.expect<void*>("Couldn't find FrameStageNotify"),
			reinterpret_cast<void*>(FrameStageNotify::hookFunc));
		ShouldShowCrosshair::hook.emplace(
			Memory::emalloc,
			Memory::shouldShowCrosshair,
			reinterpret_cast<void*>(ShouldShowCrosshair::hookFunc));
		FireEvent::hook.emplace(
			Memory::emalloc,
			Memory::fireEvent,
			reinterpret_cast<void*>(FireEvent::hookFunc));

		FrameStageNotify::hook->enable();
		ShouldShowCrosshair::hook->enable();
		FireEvent::hook->enable();
	}

	void destroy()
	{
		FireEvent::hook.reset();
		ShouldShowCrosshair::hook.reset();
		FrameStageNotify::hook.reset();
	}
}
