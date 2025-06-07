#pragma once

#include <concepts>
#include <utility>

union NodeResult {
	float f;
	bool b;

	static NodeResult create(float f) { return { .f = f }; }
	static NodeResult create(bool b) { return { .b = b }; }
};

template <typename T>
static constexpr T unwrap_node_result(NodeResult nr)
{
	if constexpr (std::same_as<T, float>)
		return nr.f;
	else if constexpr (std::same_as<T, bool>)
		return nr.b;
	std::unreachable();
}
