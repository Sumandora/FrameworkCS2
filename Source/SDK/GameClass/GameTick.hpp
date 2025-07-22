#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../TickTimeConversion.hpp"

#include <cstdint>

struct GameTick {
	CLASS_INFO("!GlobalTypes", "GameTick_t");

	SCHEMA_VAR(std::int32_t, value, "m_Value");

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	operator int32_t() const
	{
		return value();
	}

	[[nodiscard]] float as_time() const {
		return ticks_to_time(value());
	}
};
