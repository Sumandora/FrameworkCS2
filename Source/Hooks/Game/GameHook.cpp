#include "GameHook.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "DetourHooking.hpp"

#include "../../SDK/GameClass/ClientModeCSNormal.hpp"
#include "../../SDK/GameClass/CSGOInput.hpp"
#include "../../SDK/GameClass/Source2Client.hpp"

#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

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
		AddSleeveModel::hook.emplace(
			Memory::emalloc,
			BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"sleeve_model">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				.sub(6)
				.filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"44 89 ff">()); })
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 31 c9 ba">())
				.expect<void*>("Couldn't find add sleeve model function"),
			reinterpret_cast<void*>(AddSleeveModel::hook_func));
		DrawArrayExt::hook.emplace(
			Memory::emalloc,
			// This function is CAnimatableSceneObjectDesc's 2nd vfunc, TODO maybe I should scan for that instead.
			BCRL::signature(
				Memory::mem_mgr,
				// This is a 128-bit operand stored at a memory address, which is then accessed through relative reference
				SignatureScanner::PatternSignature::for_array_of_bytes<"57 79 d4 09 57 79 d4 09 57 79 d4 09 57 79 d4 09">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libscenesystem.so"))
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libscenesystem.so"))
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				.expect<void*>("Couldn't find draw object function"),
			reinterpret_cast<void*>(DrawArrayExt::hook_func));
		SyncViewAngles::hook.emplace(
			Memory::emalloc,
			BCRL::pointer_array(Memory::mem_mgr, Memory::csgo_input, CSGOInput::sync_view_angles_index)
				.expect<void*>("Couldn't find SyncViewAngles"),
			reinterpret_cast<void*>(SyncViewAngles::hook_func));
		EmitSound::hook.emplace(
			Memory::emalloc,
			BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"player_sound">(), // lovely
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				// This is not the beginning of the function, but it doesn't matter.
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				.expect<void*>("Couldn't find EmitSound"),
			reinterpret_cast<void*>(EmitSound::hook_func));
		OverrideView::hook.emplace(
			Memory::emalloc,
			BCRL::pointer_array(
				Memory::mem_mgr,
				Memory::client_mode_cs_normal,
				ClientModeCSNormal::override_view_index)
				.expect<void*>("Couldn't find OverrideView"),
			reinterpret_cast<void*>(OverrideView::hook_func));
		UpdateBombRadius::hook.emplace(
			Memory::emalloc,
			BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"bombradius">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 31 d2 48 89 e5">())
				.expect<void*>("Couldn't find UpdateBombRadius"),
			reinterpret_cast<void*>(UpdateBombRadius::hook_func));
		OnVoteStart::hook.emplace(
			Memory::emalloc,
			BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"#Panorama_Vote_Server">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				.find_xrefs(SignatureScanner::XRefTypes::relative(),
					BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				.prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				.expect<void*>("Couldn't find OnVoteStart"),
			reinterpret_cast<void*>(OnVoteStart::hook_func));

		// There is a relative offset at the beginning of this function
		// TODO This needs to be addressed in DetourHooking, although I don't really want to...

		void* particle_draw_array
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"CParticleObjectDesc">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libparticles.so"))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libparticles.so"))
				  .sub(3)
				  .find_xrefs(
					  SignatureScanner::XRefTypes::absolute(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libparticles.so"))
				  .add(8)
				  .dereference()
				  .expect<void*>("Couldn't find CParticleObjectDesc::DrawArray");

		std::byte buf[13];
		Memory::mem_mgr.read(reinterpret_cast<std::uintptr_t>(particle_draw_array), &buf, 13);
		std::byte buf2[7];
		std::memcpy(buf2, &buf[1], 7);
		std::memcpy(&buf[1], &buf[8], 5);
		std::memcpy(&buf[6], buf2, 7);
		*reinterpret_cast<std::int32_t*>(buf + 9) -= 5;
		Memory::mem_mgr.write(reinterpret_cast<std::uintptr_t>(particle_draw_array), &buf, 13);

		ParticlesDrawArray::hook.emplace(
			Memory::emalloc,
			particle_draw_array,
			reinterpret_cast<void*>(ParticlesDrawArray::hook_func));

		FrameStageNotify::hook->enable();
		ShouldShowCrosshair::hook->enable();
		FireEvent::hook->enable();
		GetFunLoading::hook->enable();
		CreateMove::hook->enable();
		RadarUpdate::hook->enable();
		RenderLegs::hook->enable();
		AddSleeveModel::hook->enable();
		DrawArrayExt::hook->enable();
		SyncViewAngles::hook->enable();
		EmitSound::hook->enable();
		OverrideView::hook->enable();
		UpdateBombRadius::hook->enable();
		OnVoteStart::hook->enable();
		ParticlesDrawArray::hook->enable();
	}

	void destroy()
	{
		ParticlesDrawArray::hook.reset();
		OnVoteStart::hook.reset();
		UpdateBombRadius::hook.reset();
		OverrideView::hook.reset();
		EmitSound::hook.reset();
		SyncViewAngles::hook.reset();
		DrawArrayExt::hook.reset();
		AddSleeveModel::hook.reset();
		RenderLegs::hook.reset();
		RadarUpdate::hook.reset();
		CreateMove::hook.reset();
		GetFunLoading::hook.reset();
		FireEvent::hook.reset();
		ShouldShowCrosshair::hook.reset();
		FrameStageNotify::hook.reset();
	}
}
