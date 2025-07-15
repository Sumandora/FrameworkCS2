#include "String.hpp"

#include "../../SDK/GameClass/MemAlloc.hpp"

UtlString::~UtlString()
{
	MemAlloc::the()->deallocate(string);
}
