#pragma once

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <vector>

#include "IdType.hpp"
#include "Node.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "Nodes/OriginalInputNode.hpp"
#include "Nodes/OutputNode.hpp"

#include "nlohmann/json.hpp"

class NodeCircuit {
	IdType id_counter = 1;

	std::unordered_map<IdType, Node*> ids;
	struct Link {
		IdType start_node, start_attribute;
		IdType end_node, end_attribute;
	};
	std::vector<Link> links;

	OriginalInputNode original_input_node;
	OutputNode output_node;

	void render_new_menu();
	void center_editor();

public:
	NodeCircuit(NodeType type, std::function<NodeResult()> get_original_value);

	void push_node(IdType id, Node* node) { ids[id] = node; }
	void render(bool newly_opened);

	// Returns if original input links directly to output.
	[[nodiscard]] bool is_trivial() const;

	[[nodiscard]] IdType next_id() { return id_counter++; }
	[[nodiscard]] Node* from_id(IdType id) const;
	[[nodiscard]] NodeResult get_output() const { return output_node.get_value(); }

	void serialize(const nlohmann::json& input_json) const;
	void deserialize(const nlohmann::json& input_json);
};
