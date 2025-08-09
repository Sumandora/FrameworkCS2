#pragma once

#include "imgui.h"

#include <cstdint>

struct RGBA32 {
	std::uint8_t r, g, b, a;

	static RGBA32 from_imgui(ImColor color)
	{
		return {
			.r = static_cast<std::uint8_t>(color.Value.x * 255),
			.g = static_cast<std::uint8_t>(color.Value.y * 255),
			.b = static_cast<std::uint8_t>(color.Value.z * 255),
			.a = static_cast<std::uint8_t>(color.Value.w * 255),
		};
	}

	bool operator==(RGBA32 other) const
	{
		return r == other.r
			&& g == other.g
			&& b == other.b
			&& a == other.a;
	}
};

static_assert(sizeof(RGBA32) == 0x4);
