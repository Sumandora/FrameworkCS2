#include "ComparisonNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "magic_enum/magic_enum.hpp"

#include <cctype>
#include <cmath>
#include <string>
#include <utility>

ComparisonNode::ComparisonNode(NodeCircuit* parent, ComparisonOp operation)
	: Node(parent, replace_underscores_with_spaces(std::string{ magic_enum::enum_name(operation) }), NodeType::BOOLEAN)
	, lhs(parent->next_id())
	, rhs(parent->next_id())
	, output(parent->next_id())
	, operation(operation)
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
