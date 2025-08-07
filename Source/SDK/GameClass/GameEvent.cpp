#include "GameEvent.hpp"

#include "murmurhash/MurmurHash2.h"

#include <alloca.h>
#include <cstdint>
#include <cstring>
#include <string_view>

static constexpr std::uint32_t STRINGTOKEN_SEED = 0x31415926;

int GameEvent::get_int(std::string_view key_name, int default_value) const
{
	struct HashedName {
		std::uint32_t hash;
		const char* string;
	};

	HashedName hashed;

	hashed.hash = MurmurHash2(key_name.data(), static_cast<int>(key_name.length()), STRINGTOKEN_SEED);
	hashed.string = key_name.data();
	return get_int_with_hash(&hashed, default_value);
}

const char* GameEvent::get_string(std::string_view key_name, const char* default_value) const
{
	struct HashedName {
		std::uint32_t hash;
		const char* string;
	};

	HashedName hashed;

	hashed.hash = MurmurHash2(key_name.data(), static_cast<int>(key_name.length()), STRINGTOKEN_SEED);
	hashed.string = key_name.data();
	return get_string_with_hash(&hashed, default_value);
}

int GameEvent::get_userid(std::string_view key_name, int default_value) const
{
	struct HashedName {
		std::uint32_t hash;
		const char* string;
	};

	HashedName hashed;

	hashed.hash = MurmurHash2(key_name.data(), static_cast<int>(key_name.length()), STRINGTOKEN_SEED);
	hashed.string = key_name.data();

	return get_userid_with_hash(&hashed, default_value);
}

BaseEntity* GameEvent::get_entity(std::string_view key_name) const
{
	struct HashedName {
		std::uint32_t hash;
		const char* string;
	};

	HashedName hashed;

	hashed.hash = MurmurHash2(key_name.data(), static_cast<int>(key_name.length()), STRINGTOKEN_SEED);
	hashed.string = key_name.data();

	return get_entity_with_hash(&hashed);
}
