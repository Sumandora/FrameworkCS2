#pragma once

#include "IdType.hpp"
#include "Node.hpp"
#include "Nodes/ArithmeticNode.hpp"
#include "Nodes/BooleanValueNode.hpp"
#include "Nodes/ComparisonNode.hpp"
#include "Nodes/FloatValueNode.hpp"
#include "Nodes/OriginalInputNode.hpp"
#include "Nodes/OutputNode.hpp"

#include <concepts>
#include <cstddef>
#include <tuple>

using AllNodes = std::tuple<
	OriginalInputNode,
	OutputNode,
	BooleanValueNode,
	FloatValueNode,
	ArithmeticNode,
	ComparisonNode>;

namespace detail {
	// https://stackoverflow.com/a/18063608
	template <class T, class Tuple>
	struct Index;

	template <class T, class... Types>
	struct Index<T, std::tuple<T, Types...>> {
		static const std::size_t VALUE = 0;
	};

	template <class T, class U, class... Types>
	struct Index<T, std::tuple<U, Types...>> {
		static const std::size_t VALUE = 1 + Index<T, std::tuple<Types...>>::VALUE;
	};
}

template <typename T>
static constexpr std::size_t NODE_ID = detail::Index<T, AllNodes>::VALUE;

template <std::size_t Index = 0>
constexpr Node* instantiate_node_by_id(NodeCircuit* circuit, const nlohmann::json& json, std::size_t node_id)
{
	using T = std::tuple_element_t<Index, AllNodes>;
	if constexpr (!std::same_as<T, OriginalInputNode> && !std::same_as<T, OutputNode>)
		if (NODE_ID<T> == node_id)
			return T::deserialize(circuit, json);

	return instantiate_node_by_id<Index + 1>(circuit, json, node_id);
}

template <>
constexpr Node* instantiate_node_by_id<std::tuple_size_v<AllNodes>>(NodeCircuit* /*circuit*/, const nlohmann::json& /*json*/, std::size_t /*node_id*/)
{
	return nullptr;
}
