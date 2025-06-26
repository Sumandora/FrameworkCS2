#pragma once

#include "../Padding.hpp"

#include "cs_usercmd.pb.h" // IWYU pragma: export
#include "network_connection.pb.h" // IWYU pragma: export
#include "networkbasetypes.pb.h" // IWYU pragma: export
#include "usercmd.pb.h" // IWYU pragma: export

#include <cstdint>

struct BasePlayerController;

static constexpr std::uint64_t IN_JUMP = 1 << 1;

struct Buttons {
	PADDING(0x8);
	std::uint64_t buttonstate1;
	std::uint64_t buttonstate2;
	std::uint64_t buttonstate3;
};

struct UserCmd {
private:
	PADDING(0x10); // uh...

public:
	CSGOUserCmdPB csgo_usercmd;
	Buttons buttons;

private:
	PADDING(32);

public:
	static void resolve_signatures();
	static UserCmd* get_current_command(BasePlayerController* controller);

	CSubtickMoveStep* allocate_new_move_step(float when);
};

static_assert(sizeof(UserCmd) == 0x98);
