#pragma once

#include <cstdint>

namespace Hooks {

	void Create();
	void Destroy();

	std::size_t CalculateHookLength(void* address);

}