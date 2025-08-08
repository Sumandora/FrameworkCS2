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
#include "../../SDK/GameClass/LightBinnerGPU.hpp"
#include "../../SDK/GameClass/RenderingPipelineCsgoPostHud.hpp"
#include "../../SDK/GameClass/Source2Client.hpp"

#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Hooks::Game {
	void create()
	{
		void* frame_stage_notify
			= BCRL::pointer_array(
				Memory::mem_mgr,
				Interfaces::source2_client,
				Source2Client::frame_stage_notify_index)
				  .BCRL_EXPECT(void*, frame_stage_notify);

		void* should_show_crosshair
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"weapon_reticle_knife_show">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"4c 8d 3d ? ? ? ? b9">())
				  .add(3)
				  .relative_to_absolute() // This is a ConVar instance (not the ConVar type in the SDK)
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
				  .add(4)
				  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"be ff ff ff ff">()); })
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				  .BCRL_EXPECT(void*, should_show_crosshair);

		void* fire_event
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"FireEvent: event '%s' not registered.\n">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
				  .sub(3)
				  .filter([](const auto& ptr) {
					  return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 3d">());
				  })
				  // What is the lea in the middle of the prologue?
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 8d 87 ? ? ? ? 48 89 e5">())
				  .BCRL_EXPECT(void*, fire_event);

		void* get_fun_loading
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"#LoadingProgress_CSFunLoading%d">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().thats_not_executable().with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
				  .sub(3)
				  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 35">()); })
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				  .BCRL_EXPECT(void*, get_fun_loading);

		void* create_move
			= BCRL::pointer_array(
				Memory::mem_mgr,
				Memory::csgo_input,
				CSGOInput::create_move_index)
				  .BCRL_EXPECT(void*, create_move);

		void* radar_update
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"cl_radar_square_with_scoreboard">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				  .sub(21)
				  .relative_to_absolute()
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				  .sub(3)
				  .filter([](const auto& ptr) {
					  return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 3d ? ? ? ? be ff ff ff ff">());
				  })
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				  .BCRL_EXPECT(void*, radar_update);

		void* render_legs
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"FirstpersonLegsPrepass">(),
				BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
				  .sub(3)
				  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 35">()); })
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				  .BCRL_EXPECT(void*, render_legs);

		void* draw_array_ext
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"CAnimatableSceneObjectDesc">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libscenesystem.so"))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libscenesystem.so"))
				  .sub(3)
				  .find_xrefs(
					  SignatureScanner::XRefTypes::absolute(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libscenesystem.so"))
				  .add(sizeof(void*) * 2)
				  .dereference()
				  .BCRL_EXPECT(void*, draw_array_ext);

		void* sync_view_angles
			= BCRL::pointer_array(
				Memory::mem_mgr,
				Memory::csgo_input,
				CSGOInput::sync_view_angles_index)
				  .BCRL_EXPECT(void*, sync_view_angles);

		void* emit_sound
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"player_sound">(), // lovely
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  // This is not the beginning of the function, but it doesn't matter.
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 b8 00 00 00 00 ff ff ff ff">())
				  .BCRL_EXPECT(void*, emit_sound);

		void* override_view
			= BCRL::pointer_array(
				Memory::mem_mgr,
				Memory::client_mode_cs_normal,
				ClientModeCSNormal::override_view_index)
				  .BCRL_EXPECT(void*, override_view);

		void* on_vote_start
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"#Panorama_Vote_Server">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  .sub(3)
				  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 35">()); })
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				  .BCRL_EXPECT(void*, on_vote_start);

		void* particles_draw_array
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
				  .add(sizeof(void*))
				  .dereference()
				  .BCRL_EXPECT(void*, particles_draw_array);

		void* light_binner
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"15CLightBinnerGPU">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libscenesystem.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::absolute(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libscenesystem.so"))
				  .sub(8)
				  .find_xrefs(SignatureScanner::XRefTypes::absolute(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libscenesystem.so"))
				  .add(sizeof(void*)) // skip pointer to rtti
				  .add(LightBinnerGPU::process_lights_index * sizeof(void*))
				  .dereference()
				  // Unfortunate jmp:
				  .add(1)
				  .relative_to_absolute()
				  .BCRL_EXPECT(void*, light_binner);

		void* draw_hud_overlay
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"Health Boost Effect">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .find_xrefs(SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 66 0f ef c0 48 89 e5">())
				  .BCRL_EXPECT(void*, draw_hud_overlay);

		void* calculate_viewmodel_position
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"Viewmodel FOV">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 05">())
				  .add(3)
				  .relative_to_absolute() // viewmodel_fov convar
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  .add(4)
				  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()); })
				  .repeater([](auto& ptr) {
					  ptr.next_instruction();
					  static int i = 0;
					  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
						  i++;
					  return i != 2;
				  })
				  .add(1)
				  .relative_to_absolute()
				  .BCRL_EXPECT(void*, calculate_viewmodel_position);

		void* get_fov
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"fov_cs_debug">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 05">())
				  .add(3)
				  .relative_to_absolute() // viewmodel_fov convar
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  .add(4)
				  .filter([](const auto& ptr) { return ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"be ff ff ff ff">()); })
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				  .BCRL_EXPECT(void*, get_fov);

		auto apply_glow_session
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"22ISceneObjectController">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::absolute(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .sub(sizeof(char*))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
				  .flat_map([](const auto& ptr) {
					  auto ptrs = ptr.find_xrefs(
						  SignatureScanner::XRefTypes::relative(),
						  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"));
					  if (ptrs.size() > 1) {
						  return std::vector<std::remove_cvref_t<decltype(ptr)>>{};
					  }
					  return ptrs;
				  })
				  // Here is a basic block which is invoked right after the function that we want.
				  .sub(7)
				  .find_xrefs(
					  SignatureScanner::XRefTypes::relative(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
				  // Now we are in the right basic block
				  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"e8 ? ? ? ? 49">())
				  .add(1)
				  // What a journey...
				  .relative_to_absolute();

		void* is_glowing
			= apply_glow_session
				  .clone()
				  .repeater([](auto& ptr) {
					  ptr.next_instruction();
					  return !ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">());
				  })
				  .add(1)
				  .relative_to_absolute()
				  .BCRL_EXPECT(void*, is_glowing);

		void* get_glow_color
			= apply_glow_session
				  .clone()
				  .repeater([](auto& ptr) {
					  ptr.next_instruction();
					  static int i = 0;
					  if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"e8">()))
						  i++;
					  return i != 3;
				  })
				  .add(1)
				  .relative_to_absolute()
				  .BCRL_EXPECT(void*, get_glow_color);

		void* add_layers_post_hud
			= BCRL::signature(
				Memory::mem_mgr,
				SignatureScanner::PatternSignature::for_literal_string<"29CRenderingPipelineCsgoPostHud">(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::absolute(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .sub(sizeof(char*))
				  .find_xrefs(
					  SignatureScanner::XRefTypes::absolute(),
					  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
				  .add(sizeof(void*))
				  .add(sizeof(void*) * RenderingPipelineCsgoPostHud::add_layers_index)
				  .dereference()
				  .BCRL_EXPECT(void*, add_layers_post_hud);

		FrameStageNotify::hook.emplace(
			Memory::emalloc,
			frame_stage_notify,
			reinterpret_cast<void*>(FrameStageNotify::hookFunc));
		ShouldShowCrosshair::hook.emplace(
			Memory::emalloc,
			should_show_crosshair,
			reinterpret_cast<void*>(ShouldShowCrosshair::hookFunc));
		FireEvent::hook.emplace(
			Memory::emalloc,
			fire_event,
			reinterpret_cast<void*>(FireEvent::hookFunc));
		GetFunLoading::hook.emplace(
			Memory::emalloc,
			get_fun_loading,
			reinterpret_cast<void*>(GetFunLoading::hook_func));
		CreateMove::hook.emplace(
			Memory::emalloc,
			create_move,
			reinterpret_cast<void*>(CreateMove::hook_func));
		RadarUpdate::hook.emplace(
			Memory::emalloc,
			radar_update,
			reinterpret_cast<void*>(RadarUpdate::hook_func));
		RenderLegs::hook.emplace(
			Memory::emalloc,
			render_legs,
			reinterpret_cast<void*>(RenderLegs::hook_func));
		DrawArrayExt::hook.emplace(
			Memory::emalloc,
			draw_array_ext,
			reinterpret_cast<void*>(DrawArrayExt::hook_func));
		SyncViewAngles::hook.emplace(
			Memory::emalloc,
			sync_view_angles,
			reinterpret_cast<void*>(SyncViewAngles::hook_func));
		EmitSound::hook.emplace(
			Memory::emalloc,
			emit_sound,
			reinterpret_cast<void*>(EmitSound::hook_func));
		OverrideView::hook.emplace(
			Memory::emalloc,
			override_view,
			reinterpret_cast<void*>(OverrideView::hook_func));
		OnVoteStart::hook.emplace(
			Memory::emalloc,
			on_vote_start,
			reinterpret_cast<void*>(OnVoteStart::hook_func));
		ParticlesDrawArray::hook.emplace(
			Memory::emalloc,
			particles_draw_array,
			reinterpret_cast<void*>(ParticlesDrawArray::hook_func));
		LightBinner::hook.emplace(
			Memory::emalloc,
			light_binner,
			reinterpret_cast<void*>(LightBinner::hook_func));
		DrawHudOverlay::hook.emplace(
			Memory::emalloc,
			draw_hud_overlay,
			reinterpret_cast<void*>(DrawHudOverlay::hook_func));
		CalculateViewModelPosition::hook.emplace(
			Memory::emalloc,
			calculate_viewmodel_position,
			reinterpret_cast<void*>(CalculateViewModelPosition::hook_func));
		GetFov::hook.emplace(
			Memory::emalloc,
			get_fov,
			reinterpret_cast<void*>(GetFov::hook_func));
		IsGlowing::hook.emplace(
			Memory::emalloc,
			is_glowing,
			reinterpret_cast<void*>(IsGlowing::hook_func));
		GetGlowColor::hook.emplace(
			Memory::emalloc,
			get_glow_color,
			reinterpret_cast<void*>(GetGlowColor::hook_func));
		AddLayersPostHud::hook.emplace(
			Memory::emalloc,
			add_layers_post_hud,
			reinterpret_cast<void*>(AddLayersPostHud::hook_func));

		FrameStageNotify::hook->enable();
		ShouldShowCrosshair::hook->enable();
		FireEvent::hook->enable();
		GetFunLoading::hook->enable();
		CreateMove::hook->enable();
		RadarUpdate::hook->enable();
		RenderLegs::hook->enable();
		DrawArrayExt::hook->enable();
		SyncViewAngles::hook->enable();
		// EmitSound::hook->enable();
		OverrideView::hook->enable();
		OnVoteStart::hook->enable();
		ParticlesDrawArray::hook->enable();
		LightBinner::hook->enable();
		DrawHudOverlay::hook->enable();
		CalculateViewModelPosition::hook->enable();
		GetFov::hook->enable();
		IsGlowing::hook->enable();
		GetGlowColor::hook->enable();
		AddLayersPostHud::hook->enable();
	}

	void destroy()
	{
		AddLayersPostHud::hook.reset();
		GetGlowColor::hook.reset();
		IsGlowing::hook.reset();
		GetFov::hook.reset();
		CalculateViewModelPosition::hook.reset();
		DrawHudOverlay::hook.reset();
		LightBinner::hook.reset();
		ParticlesDrawArray::hook.reset();
		OnVoteStart::hook.reset();
		OverrideView::hook.reset();
		// EmitSound::hook.reset();
		SyncViewAngles::hook.reset();
		DrawArrayExt::hook.reset();
		RenderLegs::hook.reset();
		RadarUpdate::hook.reset();
		CreateMove::hook.reset();
		GetFunLoading::hook.reset();
		FireEvent::hook.reset();
		ShouldShowCrosshair::hook.reset();
		FrameStageNotify::hook.reset();
	}
}
