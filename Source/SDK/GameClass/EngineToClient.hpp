#pragma once

#include "../VirtualMethod.hpp"
#include <cstdint>

struct EngineToClient {
	void execute_client_cmd(const char* command)
	{
		execute_client_cmd(0, command, 1);
	}

	VIRTUAL_METHOD(93, get_local_player_index, int, (), (this));

private:
	VIRTUAL_METHOD(45, execute_client_cmd, void, (std::uint64_t rdi, const char* command, std::uint64_t rdx), (this, rdi, command, rdx));
};
