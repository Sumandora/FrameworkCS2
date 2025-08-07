#include "EngineCvar.hpp"

#include "ConVar.hpp"

#include "../../Utils/Logging.hpp"

#include <cstring>
#include <string_view>

ConVar* EngineCvar::findByName(const char* name)
{
	// TODO hashmap name -> ConVar
	for (int i = 0; i < (convar_count & 0x7fff); i++) {
		auto& list_elem = convarList.memory.memory[i];
		if (std::string_view{ list_elem.element->name } == name)
			return list_elem.element;
	}

	Logging::error("Failed to find convar '{}'", name);
	return nullptr;
}
