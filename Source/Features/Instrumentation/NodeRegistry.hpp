#pragma once

#include "tsl/ordered_map.h"

#include <cstddef>
#include <functional>
#include <string>
#include <type_traits>

class Node;
class NodeCircuit;

class NodeRegistry {
	NodeCircuit* parent;

	struct FactoryPair {
		std::function<Node*(NodeCircuit*)> create_initialized;
		std::function<Node*(NodeCircuit*)> create_uninitialized; // Used during serialization
	};

	tsl::ordered_map<std::string, FactoryPair> menu;

public:
	explicit NodeRegistry(NodeCircuit* parent);

	void render_menu() const;

	[[nodiscard]] Node* create_by_name(const std::string& name) const;

	// @arg name slash separated set of strings making up the path taken through the menu
	void add_node_type(std::string name, FactoryPair factory);

	template <typename NewNode>
		requires std::is_base_of_v<Node, NewNode>
	void add_node_by_type()
	{
		return add_node_type(
			std::string{ NewNode::NAME },
			{
				.create_initialized = NewNode::initialized,
				.create_uninitialized = NewNode::uninitialized,
			});
	}
};
