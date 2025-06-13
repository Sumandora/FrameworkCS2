#pragma once

#include <concepts>
#include <cstdint>
#include <string>
#include <type_traits>

enum class NodeType : std::uint8_t {
	NOTHING, // eh, hacky
	ANYTHING,

	FLOAT,
	BOOLEAN,
	COLOR,
	ENUM,
	STRING,
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
	else if constexpr (std::is_enum_v<T>)
		return NodeType::ENUM;
	else if constexpr (std::same_as<std::string, T>)
		return NodeType::STRING;
	else
		static_assert(false);
}
