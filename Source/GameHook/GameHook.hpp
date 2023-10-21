#pragma once

#include "../SDK/GameClass/ClientFrameStage.hpp"
#include "../SDK/GameClass/GameEvent.hpp"

#include "DetourHooking.hpp"

namespace GameHook {
	void hook();
	void unhook();

	class GameHook {
		DetourHooking::Hook* backingHook;

	public:
		void* proxy;

		GameHook() = delete;
		GameHook(void* original, void* hook);
		~GameHook();
	};

	namespace FrameStageNotify {
		inline GameHook* hook;
		void hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage);
	}

	namespace ShouldShowCrosshair {
		inline GameHook* hook;
		bool hookFunc([[maybe_unused]] void* weapon);
	}

	namespace FireEvent {
		inline GameHook* hook;
		void* hookFunc([[maybe_unused]] void* gameEventManager, GameEvent* event, [[maybe_unused]] bool rdx, [[maybe_unused]] bool rcx);
	}
}