#pragma once

#include <cstdint>

enum class NodeType : std::uint8_t {
	NOTHING, // eh, hacky
	FLOAT,
	BOOLEAN
};

template <typename T>
struct NodeTypeFor { };

template <>
struct NodeTypeFor<float> {
	static constexpr NodeType TYPE = NodeType::FLOAT;
};

template <>
struct NodeTypeFor<bool> {
	static constexpr NodeType TYPE = NodeType::BOOLEAN;
};
