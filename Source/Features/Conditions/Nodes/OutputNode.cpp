#include "OutputNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"
#include "../Nodes.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cstddef>
#include <iterator>
#include <utility>

OutputNode::OutputNode(NodeCircuit* parent, NodeType type)
	: Node(parent, "Output", NodeType::NOTHING)
	, input(parent->next_id())
	, type(type)
{
}

void OutputNode::render_io()
{
	ImNodes::BeginInputAttribute(input);
	ImGui::TextUnformatted("input");
	ImNodes::EndInputAttribute();
};

NodeResult OutputNode::get_value() const
{
	Node* node = get_parent()->from_id(input);
	if (!node)
		return {};
	return node->get_value();
}

std::size_t OutputNode::node_id() const
{
	return NODE_ID<OutputNode>;
}

void OutputNode::serialize(nlohmann::json& /*output_json*/) const {
	std::unreachable();
}
