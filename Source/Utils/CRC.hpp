#pragma once

#include "../SDK/CUtl/Buffer.hpp"

struct UserCmd;

namespace CRC {
	void update_crc(UserCmd* usercmd);
}
