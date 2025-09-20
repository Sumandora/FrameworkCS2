#pragma once

#include "../Padding.hpp"
#include "../VirtualMethod.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstdint>

// TODO No clue where windows folks get this name from...
struct NetworkedClientInfo {
	PADDING(0x4);
	std::int32_t render_tick_count;
	float render_tick_fraction;
	std::int32_t player_tick_count;
	float player_tick_fraction;

	// Same for this name, no clue where they get it from...
	struct LocalData {
		glm::vec3 viewangles;
		glm::vec3 eye_pos;
	};
	PADDING(0x4);
	LocalData* local_data;
	PADDING(0x8);
};

static_assert(sizeof(NetworkedClientInfo) >= 0x28); // Might be bigger but get_network_client_info doesn't write that much...

struct EngineToClient {
	void execute_client_cmd(const char* command)
	{
		execute_client_cmd(0, command, 1);
	}

	NetworkedClientInfo get_network_client_info()
	{
		NetworkedClientInfo d;
		get_network_client_info(&d);
		return d;
	}

private:
	VIRTUAL_METHOD(49, execute_client_cmd, void, (std::uint64_t rdi, const char* command, std::uint64_t rdx), (this, rdi, command, rdx));

	STATIC_VIRTUAL_METHOD(179, get_network_client_info, NetworkedClientInfo*, (NetworkedClientInfo * info), (this, info));
};
