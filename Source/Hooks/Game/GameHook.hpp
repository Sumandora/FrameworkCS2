#pragma once

#include "../../Memory.hpp"

#include "../../SDK/GameClass/ClientFrameStage.hpp"

#include "DetourHooking.hpp"

struct GameEvent;

namespace Hooks::Game {
	void create();
	void destroy();

	class GameHook {
		DetourHooking::Hook<true, decltype(Memory::mem_mgr)> backingHook;

	public:
		GameHook() = delete;
		GameHook(void* original, void* hook);
		~GameHook();

		[[nodiscard]] void* get_proxy() const noexcept;
	};

	namespace FrameStageNotify {
		inline std::unique_ptr<GameHook> hook;
		void hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage);
	}

	namespace ShouldShowCrosshair {
		inline std::unique_ptr<GameHook> hook;
		bool hookFunc([[maybe_unused]] void* weapon);
	}

	namespace FireEvent {
		inline std::unique_ptr<GameHook> hook;
		void* hookFunc([[maybe_unused]] void* gameEventManager, GameEvent* event, [[maybe_unused]] bool rdx, [[maybe_unused]] bool rcx);
	}
}
