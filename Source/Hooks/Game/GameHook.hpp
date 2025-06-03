#pragma once

#include "../../SDK/GameClass/ClientFrameStage.hpp"

#include "../../Utils/ThreadExecutor.hpp"
#include "../../Utils/UninitializedObject.hpp"

#include "../Hooks.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>

struct GameEvent;

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
		void* hookFunc([[maybe_unused]] void* gameEventManager, GameEvent* event, [[maybe_unused]] bool rdx, [[maybe_unused]] bool rcx);

		inline std::unordered_map<std::string, std::size_t> event_counters;
	}

	namespace GetFunLoading {
		inline UninitializedObject<DetourHook<true>> hook;
		const char* hook_func(const char* fallback);
	}

	namespace CreateMove {
		inline UninitializedObject<DetourHook<true>> hook;
		// TODO does this actually return void? lets go with void* for now.
		void* hook_func(void* csgo_input, int unk, void* usercmd);
	}
}
