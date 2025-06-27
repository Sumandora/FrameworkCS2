#include "ComparisonNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cctype>
#include <cstddef>
#include <utility>

ComparisonNode::ComparisonNode(NodeCircuit* parent, ComparisonOp operation, IdType lhs, IdType rhs, IdType output)
	: Node(parent)
	, operation(operation)
	, lhs(lhs)
	, rhs(rhs)
	, output(output)
{
}

ComparisonNode* ComparisonNode::initialized(NodeCircuit* parent, ComparisonOp operation)
{
	return new ComparisonNode{ parent, operation, parent->next_id(), parent->next_id(), parent->next_id() };
}

ComparisonNode* ComparisonNode::uninitialized(NodeCircuit* parent, ComparisonOp operation)
{
	return new ComparisonNode{ parent, operation, 0, 0, 0 };
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

NodeResult ComparisonNode::get_value(IdType /*id*/) const
{
	const NodeResult l = get_parent()->value_from_attribute(lhs);
	const NodeResult r = get_parent()->value_from_attribute(rhs);

	if (l.empty() || r.empty())
		return {};

	const auto lhs = l.get<float>();
	const auto rhs = r.get<float>();

	switch (operation) {
	case ComparisonOp::Equals:
		return lhs == rhs;
	case ComparisonOp::Not_equals:
		return lhs != rhs;
	case ComparisonOp::Lower_than:
		return lhs < rhs;
	case ComparisonOp::Greater_than:
		return lhs > rhs;
	case ComparisonOp::Lower_or_equal_than:
		return lhs <= rhs;
	case ComparisonOp::Greater_or_equal_than:
		return lhs >= rhs;
	}

	std::unreachable();
}

void ComparisonNode::serialize(nlohmann::json& output_json) const
{
	output_json["lhs"] = lhs;
	output_json["rhs"] = rhs;
	output_json["output"] = output;
}

void ComparisonNode::deserialize(const nlohmann::json& input_json)
{
	lhs = input_json["lhs"];
	rhs = input_json["rhs"];
	output = input_json["output"];
}
