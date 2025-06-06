#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>

class Node;
class NodeCircuit;

class NodeRegistry {
	NodeCircuit* parent;

	struct FactoryPair {
		std::function<Node*(NodeCircuit*)> create_initialized;
		std::function<Node*(NodeCircuit*)> create_uninitialized; // Used during serialization
	};

	std::unordered_map<std::string, FactoryPair> menu;

public:
	explicit NodeRegistry(NodeCircuit* parent);

	void render_menu() const;

	[[nodiscard]] Node* create_by_name(const std::string& name) const;

	// @arg name slash separated set of strings making up the path taken through the menu
	void add_node_type(std::string name, FactoryPair factory);
};
