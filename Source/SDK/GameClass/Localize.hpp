#pragma once

#include "../VirtualMethod.hpp"

#include <cstring>
#include <optional>
#include <string_view>

class Localize {
public:
	VIRTUAL_METHOD(18, find_safe, const char*, (const char* localization_key), (this, localization_key));

	std::optional<const char*> try_localize(const char* localization_key)
	{
		const char* translation = find_safe(localization_key);
		if (std::string_view{ translation } == localization_key) {
			return std::nullopt;
		}
		return translation;
	}
};
