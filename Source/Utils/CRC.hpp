#pragma once

#include <cstddef>
#include <vector>

struct UserCmd;

namespace CRC {
	bool update_crc(UserCmd* usercmd);
	std::vector<char> extract_crc(UserCmd* usercmd);
}
