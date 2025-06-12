#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

#include "IdType.hpp"
#include "Node.hpp"
#include "NodeRegistry.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "Nodes/OriginalInputNode.hpp"
#include "Nodes/OutputNode.hpp"

#include "nlohmann/detail/macro_scope.hpp"
#include "nlohmann/json.hpp"

struct ImNodesContext;

class NodeCircuit {
	ImNodesContext* imnodes_context;
	IdType id_counter = 1;

	std::unordered_map<IdType, Node*> ids;
	struct Link {
		IdType start_node, start_attribute;
		IdType end_node, end_attribute;

		// NOLINTNEXTLINE
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Link, start_node, start_attribute, end_node, end_attribute);
	};
	std::vector<Link> links;

	OriginalInputNode original_input_node;
	IdType original_input_node_id;
	OutputNode output_node;
	IdType output_node_id;

	NodeRegistry registry{ this };

	void center_editor();

	bool is_dynamic_node(Node*) const;

public:
	NodeCircuit(NodeType type, std::function<NodeResult()> get_original_value);
	~NodeCircuit();

	void push_node(IdType id, Node* node);
	void render(bool newly_opened);

	// Returns if original input links directly to output.
	[[nodiscard]] bool is_trivial() const;

	[[nodiscard]] IdType next_id() { return id_counter++; }
	[[nodiscard]] Node* node_from_start_attrib(IdType id) const;
	[[nodiscard]] Node* node_from_end_attrib(IdType id) const;
	[[nodiscard]] std::optional<NodeResult> value_from_attribute(IdType id) const;
	[[nodiscard]] NodeResult get_output() const { return output_node.get_value(output_node.get_input()); }

	void serialize(nlohmann::json& output_json) const;
	void deserialize(const nlohmann::json& input_json);

	NodeRegistry& get_node_registry(this auto&& self) { return self.registry; };
};
