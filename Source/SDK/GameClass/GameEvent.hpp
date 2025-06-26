#pragma once

#include "../VirtualMethod.hpp"

struct GameEvent {
	VIRTUAL_METHOD(2, GetName, char*, (), (this));

	const char* get_string(const char* key_name, const char* default_value) const;

private:
	VIRTUAL_METHOD(11, get_string_with_hash, const char*, (const void* hashed_string, const char* default_value) const, (this, hashed_string, default_value));
};
