#pragma once

#include "../../SDK/Enums/ClientFrameStage.hpp"
#include "../../SDK/NetMessages/NetMessagePB.hpp"

#include "../../Utils/ThreadExecutor.hpp"
#include "../../Utils/UninitializedObject.hpp"

#include "../Hooks.hpp"

#include "cstrike15_usermessages.pb.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

struct GameEvent;
struct GameRadar;
struct UserCmd;
struct MeshDrawPrimitive;
struct ViewSetup;
struct ViewRender;
struct MapInfo;
struct SceneLightObject;
struct HudOverlayData;

class CSGOInput;

namespace Hooks::Game {
	void create();
	void destroy();

	namespace FrameStageNotify {
		inline ThreadExecutor thread_executor;

		inline UninitializedObject<DetourHook<true>> hook;
		void hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage);
	}

	namespace ShouldShowCrosshair {
		inline UninitializedObject<DetourHook<true>> hook;
		bool hookFunc([[maybe_unused]] void* weapon);
	}

	namespace FireEvent {
		inline UninitializedObject<DetourHook<true>> hook;
		void* hookFunc([[maybe_unused]] void* game_event_manager, GameEvent* event, [[maybe_unused]] bool rdx, [[maybe_unused]] bool rcx);

		inline std::unordered_map<std::string, std::size_t> event_counters;
	}

	namespace GetFunLoading {
		inline UninitializedObject<DetourHook<true>> hook;
		const char* hook_func(const char* fallback);
	}

	namespace CreateMove {
		inline UninitializedObject<DetourHook<true>> hook;
		// TODO does this actually return void? lets go with void* for now.
		void* hook_func(CSGOInput* csgo_input, int, char);
	}

	namespace RadarUpdate {
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(GameRadar* thisptr);
	}

	namespace RenderLegs {
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(void* rdi, void* rsi, void* rdx, void* rcx, void* r8);
	}

	namespace DrawArrayExt { // TODO Rename, there are multiple DrawArrays, this one belongs to "CAnimatableSceneObjectDesc"
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(void* animatable_scene_object_desc, void* render_context, MeshDrawPrimitive* mesh_array,
			int mesh_count, void* scene_view, void* scene_layer, void* scene_system_per_frame_stats, void* material);
	}

	namespace SyncViewAngles {
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(CSGOInput* thisptr, int rsi);
	}

	namespace EmitSound {
		inline UninitializedObject<DetourHook<true>> hook;
		void* hook_func(void* return_value, void* recipient_filter, int entity_index, void* emitsound);
	}

	namespace OverrideView {
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(ViewRender* thisptr, ViewSetup* view_setup);
	}

	namespace OnVoteStart {
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(void* csgo_hud_vote, NetMessagePB<CCSUsrMsg_VoteStart>* net_message);
	}

	namespace ParticlesDrawArray {
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(void* particle_object_desc, void* render_context, MeshDrawPrimitive* mesh_array,
			int mesh_count, void* scene_view, void* scene_layer, void* scene_system_per_frame_stats);
	}

	namespace LightBinner {
		inline UninitializedObject<DetourHook<true>> hook;
		void hook_func(void* thisptr, SceneLightObject* scene_light_object, void* rdx);
	}

	namespace DrawHudOverlay {
		inline UninitializedObject<DetourHook<true>> hook;
		void* hook_func(void* rdi, void* rsi, void* rdx, void* rcx, HudOverlayData* r8);
	}
}
