#pragma once

#include "NodeType.hpp"
#include "IdType.hpp"
#include "NodeResult.hpp"

#include <string>

class NodeCircuit;

class Node {
	std::string name;
	NodeCircuit* parent;
	IdType id;
	NodeType output_type;

public:
	explicit Node(NodeCircuit* parent, std::string name, NodeType output_type);
	virtual ~Node() = default;

	virtual void render_io() = 0;

	void render_node();

	[[nodiscard]] virtual NodeResult get_value() const = 0;
	[[nodiscard]] virtual NodeType get_input_type(IdType id) const = 0;

	[[nodiscard]] const std::string& get_name() const { return name; }
	[[nodiscard]] NodeCircuit* get_parent() const { return parent; }
	[[nodiscard]] IdType get_id() const { return id; }
	[[nodiscard]] NodeType get_output_type() const { return output_type; }
};
