#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>

class Node;
class NodeCircuit;

class NodeRegistry {
	NodeCircuit* parent;

	std::unordered_map<std::string, std::function<Node*(NodeCircuit*)>> menu;

public:
	explicit NodeRegistry(NodeCircuit* parent);

	void render_menu() const;

	// @arg name slash separated set of strings making up the path taken through the menu
	void add_node_type(std::string name, std::function<Node*(NodeCircuit*)> factory);
};
