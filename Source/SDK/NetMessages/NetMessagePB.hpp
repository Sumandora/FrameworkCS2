#pragma once

#include "../Padding.hpp"

template <typename PB>
struct NetMessagePB {
	PADDING(0x30); // Unsure if this offset is always right, but I looked at a few cases and it seemed to be plausible.
	PB pb;
};
