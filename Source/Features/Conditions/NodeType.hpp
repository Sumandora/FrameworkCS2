#pragma once

#include <concepts>
#include <cstdint>

enum class NodeType : std::uint8_t {
	NOTHING, // eh, hacky
	FLOAT,
	BOOLEAN,
	COLOR,
};

template <typename T>
constexpr NodeType nodetype_for()
{
	if constexpr (std::same_as<float, T>)
		return NodeType::FLOAT;
	else if constexpr (std::same_as<bool, T>)
		return NodeType::BOOLEAN;
	else if constexpr (std::same_as<struct ImColor, T>)
		return NodeType::COLOR;
	else
		static_assert(false);
}
