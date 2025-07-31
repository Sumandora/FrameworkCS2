#pragma once

#include "ExecutableMalloc/MemoryManagerAllocator.hpp"
#include "MemoryManager/LinuxMemoryManager.hpp"

#include "glm/ext/vector_float3.hpp"

#include "Utils/Logging.hpp"

#include <string_view>

struct BaseEntity;
struct CSPlayerPawn;
struct CSPlayerController;
struct GlobalVars;

class GameEntitySystem;
class CSGOInput;
class ClientModeCSNormal;

namespace Memory {
	inline CSPlayerController** local_player_controller;

	inline CSGOInput* csgo_input;

	inline CSPlayerPawn* local_player = nullptr;

	inline GlobalVars** globals;

	inline ClientModeCSNormal* client_mode_cs_normal;

	inline MemoryManager::LinuxMemoryManager<false, true, true> mem_mgr;
	inline ExecutableMalloc::MemoryManagerAllocator emalloc{ mem_mgr };

	void create();

	float get_smoke_density_in_line(const glm::vec3& from, const glm::vec3& to);

	template<typename T>
	constexpr void accept(std::string_view name, T* address)
	{
		// This is sometimes needed for disambiguation...
		if (name.starts_with("::"))
			name = name.substr(2);

		if (address)
			Logging::info("Found {} at {}", name, reinterpret_cast<const void*>(address));
		else
			Logging::error("Failed to find {}", name);
	}

#define MEM_ACCEPT(x) ::Memory::accept(#x, x)
#define BCRL_EXPECT(ty, x)                                                  \
	expect<ty>("Found no match for " #x, "Found too many matches for " #x); \
	MEM_ACCEPT(x)
}
