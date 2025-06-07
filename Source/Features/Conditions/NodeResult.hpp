#pragma once

#include "imgui.h"

#include <concepts>

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
union NodeResult {
	float f;
	bool b;
	ImColor color;

	static NodeResult create(float f) { return { .f = f }; }
	static NodeResult create(bool b) { return { .b = b }; }
	static NodeResult create(ImColor color) { return { .color = color }; }
};

template <typename T>
static constexpr T unwrap_node_result(NodeResult nr)
{
	if constexpr (std::same_as<T, float>)
		return nr.f;
	else if constexpr (std::same_as<T, bool>)
		return nr.b;
	else if constexpr (std::same_as<T, ImColor>)
		return nr.color;
	else
		static_assert(false);
}
