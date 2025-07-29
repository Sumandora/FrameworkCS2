#pragma once

#include "MemoryManager/LinuxMemoryManager.hpp"
#include "ExecutableMalloc/MemoryManagerAllocator.hpp"
#include "glm/ext/vector_float3.hpp"

struct BaseEntity;
struct CSPlayerPawn;
struct CSPlayerController;
struct GlobalVars;

class GameEntitySystem;
class CSGOInput;
class ClientModeCSNormal;

namespace Memory {
	inline void* shouldShowCrosshair;

	inline CSPlayerController** local_player_controller;

	inline void* fireEvent;

	inline CSGOInput* csgo_input;

	inline CSPlayerPawn* local_player = nullptr;

	inline void* get_fun_loading;

	inline GlobalVars** globals;

	inline ClientModeCSNormal* client_mode_cs_normal;

	inline MemoryManager::LinuxMemoryManager<false, true, true> mem_mgr;
	inline ExecutableMalloc::MemoryManagerAllocator emalloc{mem_mgr};

	void create();

	float get_smoke_density_in_line(const glm::vec3& from, const glm::vec3& to);
}
