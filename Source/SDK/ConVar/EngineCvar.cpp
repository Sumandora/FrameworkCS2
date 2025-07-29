#include "EngineCvar.hpp"

#include "ConVar.hpp"

#include "../../Utils/Logging.hpp"

#include <cstring>

ConVar* EngineCvar::findByName(const char* name)
{
	// TODO hashmap name -> ConVar
	for (int i = 0; i < (convar_count & 0x7fff); i++) {
		auto& listElem = convarList.memory.memory[i];
		if (std::strcmp(listElem.element->name, name) == 0)
			return listElem.element;
	}

	Logging::error("Failed to find convar '{}'", name);
	return nullptr;
}
