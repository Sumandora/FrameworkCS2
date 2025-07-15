#pragma once

#include "../VirtualMethod.hpp"

#include <cstddef>

struct BufferString;
struct BaseEntity;

struct GameEvent {
	VIRTUAL_METHOD(2, GetName, char*, (), (this));

	const char* get_string(const char* key_name, const char* default_value) const;
	BaseEntity* get_entity(const char* key_name) const;

private:
	VIRTUAL_METHOD(11, get_string_with_hash, const char*, (const void* hashed_string, const char* default_value) const, (this, hashed_string, default_value));
	VIRTUAL_METHOD(18, get_entity_with_hash, BaseEntity*, (const void* hashed_string) const, (this, hashed_string));
};
