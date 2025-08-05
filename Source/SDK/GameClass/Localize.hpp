#pragma once

#include "../VirtualMethod.hpp"

#include <cstring>
#include <optional>

class Localize {
public:
	VIRTUAL_METHOD(18, find_safe, const char*, (const char* localization_key), (this, localization_key));

	// TODO This is not really translation, just localization
	std::optional<const char*> try_translate(const char* localization_key)
	{
		const char* translation = find_safe(localization_key);
		if (std::strcmp(translation, localization_key) == 0) {
			return std::nullopt;
		}
		return translation;
	}
};
