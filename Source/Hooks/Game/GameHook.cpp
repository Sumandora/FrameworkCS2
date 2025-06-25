#include "GameHook.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "DetourHooking.hpp"

#include "../../SDK/GameClass/CSGOInput.hpp"
#include "../../SDK/GameClass/Source2Client.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include <cstddef>

namespace Hooks::Game {
	void create()
	{
		FrameStageNotify::hook.emplace(
			Memory::emalloc,
			BCRL::pointer_array(Memory::mem_mgr, Interfaces::source2_client, Source2Client::frame_stage_notify_index)
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
			BCRL::pointer_array(Memory::mem_mgr, Memory::csgo_input, CSGOInput::create_move_index)
				.expect<void*>("Couldn't find CreateMove"),
			reinterpret_cast<void*>(CreateMove::hook_func));
		RadarUpdate::hook.emplace(
			Memory::emalloc,
			// TODO use the convenient convars in that function to figure out where it is.
			BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 56 41 55 41 54 49 89 FC 53 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 41 80 A4 24 ? ? ? ? FE">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
				.expect<void*>("Couldn't find radar update function"),
			reinterpret_cast<void*>(RadarUpdate::hook_func));

		FrameStageNotify::hook->enable();
		ShouldShowCrosshair::hook->enable();
		FireEvent::hook->enable();
		GetFunLoading::hook->enable();
		CreateMove::hook->enable();
		RadarUpdate::hook->enable();
	}

	void destroy()
	{
		RadarUpdate::hook.reset();
		CreateMove::hook.reset();
		GetFunLoading::hook.reset();
		FireEvent::hook.reset();
		ShouldShowCrosshair::hook.reset();
		FrameStageNotify::hook.reset();
	}
}
