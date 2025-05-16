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

	namespace EntitySystem { // TODO Reorganize the entire structure of this
		inline ::GameEntitySystem** gameEntitySystem;
		inline void* getHighestEntityIndex;
		inline void* getBaseEntity;
	}

	inline void* shouldShowCrosshair;
	inline void* getLocalPlayer;

	inline void* fireEvent;

	inline void* csgo_input;

	inline CSPlayerPawn* local_player;

	inline MemoryManager::LinuxMemoryManager<false, true, true> mem_mgr;
	inline ExecutableMalloc::MemoryManagerAllocator emalloc{mem_mgr};

	void Create();
}
