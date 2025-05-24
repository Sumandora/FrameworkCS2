#pragma once

#include "MemoryManager/LinuxMemoryManager.hpp"
#include "ExecutableMalloc/MemoryManagerAllocator.hpp"

#include "SDK/Math/Matrix.hpp"

struct ViewRender;
struct GameEntitySystem;
struct BaseEntity;
struct CSPlayerPawn;

namespace Memory {
	inline ViewRender* viewRender;
	inline VMatrix* worldToProjectionMatrix;

	inline void* shouldShowCrosshair;
	inline void* getLocalPlayer;

	inline void* fireEvent;

	inline void* csgo_input;

	inline CSPlayerPawn* local_player;

	inline MemoryManager::LinuxMemoryManager<false, true, true> mem_mgr;
	inline ExecutableMalloc::MemoryManagerAllocator emalloc{mem_mgr};

	void Create();
}
