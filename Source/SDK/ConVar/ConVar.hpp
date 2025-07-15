#pragma once

#include "../Padding.hpp"

#include <bitset>

union ConVarValue {
	char* string;
	int integer;
	float floatingPoint;
	bool boolean;
};

struct ConVar {
	char* name;
	PADDING(24);
	char* description;
	PADDING(8);
	std::bitset<32> flags;
	PADDING(8);
	ConVarValue value;
	ConVarValue value_backup;

	// Functions to mirror how the game accesses con vars:
	[[nodiscard]] float get_float() const
	{
		return value.floatingPoint;
	}
	[[nodiscard]] bool get_bool() const
	{
		return value.boolean;
	}
};
