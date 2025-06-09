#pragma once

#include "imgui.h"
#include "magic_enum/magic_enum.hpp"

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
union NodeResult {
	float f;
	bool b;
	ImColor color;
	std::size_t any; // mainly used for enums

	static NodeResult create(float f) { return { .f = f }; }
	static NodeResult create(bool b) { return { .b = b }; }
	static NodeResult create(ImColor color) { return { .color = color }; }
	template<typename E> requires std::is_enum_v<E>
	static NodeResult create(E value) { return { .any = std::to_underlying(value) }; }
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
	else if constexpr (std::is_enum_v<T> && sizeof(std::underlying_type_t<T>) <= sizeof(std::declval<NodeResult>().any))
		return magic_enum::enum_cast<T>(nr.any).value(); // Force unwrap here is unsafe, but if the optional is empty, then the node tree contains a mistake.
	else
		static_assert(false);
}
