#include "GameEvent.hpp"

#include "../Entities/GameEntitySystem.hpp"

#include "murmurhash/MurmurHash2.h"

#include "../../Utils/Logging.hpp"

#include <alloca.h>
#include <cstdint>
#include <cstring>

const char* GameEvent::get_string(const char* key_name, const char* default_value) const
{
	struct HashedName {
		std::uint32_t hash;
		const char* string;
	};

	HashedName hashed;

	static constexpr std::uint32_t STRINGTOKEN_SEED = 0x31415926;
	hashed.hash = MurmurHash2(key_name, static_cast<int>(strlen(key_name)), STRINGTOKEN_SEED);
	hashed.string = key_name;
	return get_string_with_hash(&hashed, default_value);
}

BaseEntity* GameEvent::get_entity(const char* key_name) const
{
	struct HashedName {
		std::uint32_t hash;
		const char* string;
	};

	HashedName hashed;

	static constexpr std::uint32_t STRINGTOKEN_SEED = 0x31415926;
	hashed.hash = MurmurHash2(key_name, static_cast<int>(strlen(key_name)), STRINGTOKEN_SEED);
	hashed.string = key_name;

	return get_entity_with_hash(&hashed);
}
