#pragma once

#include "../VirtualMethod.hpp"

#include <cstddef>
#include <string_view>

struct BufferString;
struct BaseEntity;

struct GameEvent {
	VIRTUAL_METHOD(2, GetName, char*, (), (this));

	[[nodiscard]] int get_int(std::string_view key_name, int default_value) const;
	[[nodiscard]] const char* get_string(std::string_view key_name, const char* default_value) const;
	[[nodiscard]] int get_userid(std::string_view key_name, int default_value) const;
	[[nodiscard]] BaseEntity* get_entity(std::string_view key_name) const;

private:
	VIRTUAL_METHOD(8, get_int_with_hash, int, (const void* hashed_string, int default_value) const, (this, hashed_string, default_value));
	VIRTUAL_METHOD(11, get_string_with_hash, const char*, (const void* hashed_string, const char* default_value) const, (this, hashed_string, default_value));
	VIRTUAL_METHOD(16, get_userid_with_hash, int, (const void* hashed_string, int default_value) const, (this, hashed_string, default_value));
	VIRTUAL_METHOD(18, get_entity_with_hash, BaseEntity*, (const void* hashed_string) const, (this, hashed_string));
};
