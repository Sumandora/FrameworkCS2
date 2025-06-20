#pragma once

#include "../Padding.hpp"

#include "cs_usercmd.pb.h" // IWYU pragma: export
#include "network_connection.pb.h" // IWYU pragma: export
#include "networkbasetypes.pb.h" // IWYU pragma: export
#include "usercmd.pb.h" // IWYU pragma: export

#include <cstdint>

struct BasePlayerController;

struct Buttons { // TODO, is this a protobuf buttons?
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
	static UserCmd* get_current_command(BasePlayerController* controller);
};

static_assert(sizeof(UserCmd) == 0x98);
