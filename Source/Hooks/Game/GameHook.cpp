#include "GameHook.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "BCRL/Session.hpp"
#include "DetourHooking.hpp"

#include <cstddef>

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
		GetFunLoading::hook.emplace(
			Memory::emalloc,
			Memory::get_fun_loading,
			reinterpret_cast<void*>(GetFunLoading::hook_func));
		CreateMove::hook.emplace(
			Memory::emalloc,
			BCRL::pointer_array(Memory::mem_mgr, Memory::csgo_input, 22)
				.expect<void*>("Couldn't find CreateMove"),
			reinterpret_cast<void*>(CreateMove::hook_func));

		FrameStageNotify::hook->enable();
		ShouldShowCrosshair::hook->enable();
		FireEvent::hook->enable();
		GetFunLoading::hook->enable();
		CreateMove::hook->enable();
	}

	void destroy()
	{
		CreateMove::hook.reset();
		GetFunLoading::hook.reset();
		FireEvent::hook.reset();
		ShouldShowCrosshair::hook.reset();
		FrameStageNotify::hook.reset();
	}
}
