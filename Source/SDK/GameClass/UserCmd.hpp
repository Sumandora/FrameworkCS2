#pragma once

#include "../Padding.hpp"

#include "cs_usercmd.pb.h" // IWYU pragma: export
#include "network_connection.pb.h" // IWYU pragma: export
#include "networkbasetypes.pb.h" // IWYU pragma: export
#include "usercmd.pb.h" // IWYU pragma: export

struct UserCmd {
private:
	PADDING(0x10); // uh...

public:
	CSGOUserCmdPB csgo_usercmd;
};
