#include "ComparisonNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"
#include "../Nodes.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "magic_enum/magic_enum.hpp"

#include <cctype>
#include <cstddef>
#include <string>
#include <utility>

ComparisonNode::ComparisonNode(NodeCircuit* parent, ComparisonOp operation, IdType lhs, IdType rhs, IdType output)
	: Node(parent, replace_underscores_with_spaces(std::string{ magic_enum::enum_name(operation) }), NodeType::BOOLEAN)
	, operation(operation)
	, lhs(lhs)
	, rhs(rhs)
	, output(output)
{
}

ComparisonNode::ComparisonNode(NodeCircuit* parent, ComparisonOp operation)
	: ComparisonNode(parent, operation, parent->next_id(), parent->next_id(), parent->next_id())
{
}

void ComparisonNode::render_io()
{

	ImNodes::BeginInputAttribute(lhs);
	ImGui::TextUnformatted("lhs");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(rhs);
	ImGui::TextUnformatted("rhs");
	ImNodes::EndInputAttribute();

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

NodeResult ComparisonNode::get_value() const
{
	const Node* l = get_parent()->from_id(lhs);
	const Node* r = get_parent()->from_id(rhs);

	if (!l || !r)
		return NodeResult{ .b = false };

	switch (operation) {
	case ComparisonOp::Equals:
		return NodeResult{ .b = l->get_value().f == r->get_value().f };
	case ComparisonOp::Not_equals:
		return NodeResult{ .b = l->get_value().f != r->get_value().f };
	case ComparisonOp::Lower_than:
		return NodeResult{ .b = l->get_value().f < r->get_value().f };
	case ComparisonOp::Greater_than:
		return NodeResult{ .b = l->get_value().f > r->get_value().f };
	case ComparisonOp::Lower_or_equal_than:
		return NodeResult{ .b = l->get_value().f <= r->get_value().f };
	case ComparisonOp::Greater_or_equal_than:
		return NodeResult{ .b = l->get_value().f >= r->get_value().f };
	}

	std::unreachable();
}

std::size_t ComparisonNode::node_id() const
{
	return NODE_ID<ComparisonNode>;
}

void ComparisonNode::serialize(nlohmann::json& output_json) const
{
	output_json["operation"] = operation;

	output_json["lhs"] = lhs;
	output_json["rhs"] = rhs;
	output_json["output"] = output;
}

ComparisonNode* ComparisonNode::deserialize(NodeCircuit* parent, const nlohmann::json& input_json)
{
	return new ComparisonNode{
		parent,
		input_json["operation"],
		input_json["lhs"],
		input_json["rhs"],
		input_json["output"]
	};
}
