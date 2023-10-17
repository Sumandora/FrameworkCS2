#include "VMT.hpp"

void** vtable(void* gameClass)
{
	return *reinterpret_cast<void***>(gameClass);
}
