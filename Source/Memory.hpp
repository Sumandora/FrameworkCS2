#pragma once

#include "MemoryManager/LinuxMemoryManager.hpp"
#include "ExecutableMalloc/MemoryManagerAllocator.hpp"

#include "glm/ext/matrix_float4x4.hpp"

struct ViewRender;
struct BaseEntity;
struct CSPlayerPawn;
struct CSPlayerController;

class GameEntitySystem;
class CSGOInput;

namespace Memory {
	inline ViewRender* viewRender;
	inline glm::mat4x4* worldToProjectionMatrix;

	inline void* shouldShowCrosshair;

	inline CSPlayerController** local_player_controller;

	inline void* fireEvent;

	inline CSGOInput* csgo_input;

	inline CSPlayerPawn* local_player;

	inline void* get_fun_loading;

	inline MemoryManager::LinuxMemoryManager<false, true, true> mem_mgr;
	inline ExecutableMalloc::MemoryManagerAllocator emalloc{mem_mgr};

	void create();
}
