#include "OutputNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"
#include "nlohmann/json_fwd.hpp"

#include <cstddef>
#include <utility>

OutputNode::OutputNode(NodeCircuit* parent, NodeType type)
	: Node(parent)
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

void OutputNode::serialize(nlohmann::json& /*output_json*/) const
{
	std::unreachable();
}

void OutputNode::deserialize(const nlohmann::json& /*input_json*/)
{
	std::unreachable();
}
