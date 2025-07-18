#pragma once

#include "../Schema/FieldOffset.hpp"

struct GameTime {
	CLASS_INFO("!GlobalTypes", "GameTime_t");

	SCHEMA_VAR(float, value, "m_Value");

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	operator float() const
	{
		return value();
	}
};
