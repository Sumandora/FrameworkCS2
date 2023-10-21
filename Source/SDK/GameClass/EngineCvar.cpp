#include "EngineCvar.hpp"
#include "ConVar.hpp"

#include <cstring>

ConVar* EngineCvar::findByName(const char* name)
{
	// TODO hashmap name -> ConVar
	auto it = convarList.head;
	while (it != convarList.INVALID_INDEX) {
		auto& listElem = convarList.memory.memory[it];
		if (std::strcmp(listElem.element->name, name) == 0)
			return listElem.element;

		it = listElem.next;
	}
	return nullptr;
}