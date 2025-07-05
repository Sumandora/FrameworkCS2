#include "GameHook.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"

#include "DetourHooking.hpp"

#include "../../SDK/GameClass/CSGOInput.hpp"
#include "../../SDK/GameClass/Source2Client.hpp"

#include <csignal>
#include <cstddef>
#include <string>
#include <unistd.h>

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
			BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"cl_radar_square_with_scoreboard">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				.add(11)
				.relative_to_absolute()
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				.sub(3)
				.filter([](const auto& ptr) {
					return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 3d ? ? ? ? be ff ff ff ff">());
				})
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				.expect<void*>("Couldn't find radar update function"),
			reinterpret_cast<void*>(RadarUpdate::hook_func));
		RenderLegs::hook.emplace(
			Memory::emalloc,
			BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"FirstpersonLegsPrepass">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				.expect<void*>("Couldn't find render legs function"),
			reinterpret_cast<void*>(RenderLegs::hook_func));

		FrameStageNotify::hook->enable();
		ShouldShowCrosshair::hook->enable();
		FireEvent::hook->enable();
		GetFunLoading::hook->enable();
		CreateMove::hook->enable();
		RadarUpdate::hook->enable();
		RenderLegs::hook->enable();
	}

	void destroy()
	{
		RenderLegs::hook.reset();
		RadarUpdate::hook.reset();
		CreateMove::hook.reset();
		GetFunLoading::hook.reset();
		FireEvent::hook.reset();
		ShouldShowCrosshair::hook.reset();
		FrameStageNotify::hook.reset();
	}
}
