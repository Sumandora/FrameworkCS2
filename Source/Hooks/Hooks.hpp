#pragma once

#include "DetourHooking.hpp"

#include "../Memory.hpp"

namespace Hooks {
	template<bool NeedsTrampoline>
	using DetourHook = DetourHooking::Hook<NeedsTrampoline, decltype(Memory::mem_mgr)>;

	void create();
	void destroy();
}
