#include "ArithmeticNode.hpp"

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
#include <cmath>
#include <cstddef>
#include <string>
#include <utility>

ArithmeticNode::ArithmeticNode(NodeCircuit* parent, ArithmeticOp operation, IdType lhs, IdType rhs, IdType output)
	: Node(parent, std::string{ magic_enum::enum_name(operation) }, NodeType::FLOAT)
	, operation(operation)
	, lhs(lhs)
	, rhs(rhs)
	, output(output)
{
}

ArithmeticNode::ArithmeticNode(NodeCircuit* parent, ArithmeticOp operation)
	: ArithmeticNode(parent, operation, parent->next_id(), parent->next_id(), parent->next_id())
{
}

void ArithmeticNode::render_io()
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

NodeResult ArithmeticNode::get_value() const
{
	const Node* l = get_parent()->from_id(lhs);
	const Node* r = get_parent()->from_id(rhs);

	if (!l || !r)
		return NodeResult{ .f = 0.0F };

	switch (operation) {
	case ArithmeticOp::Addition:
		return NodeResult{ .f = l->get_value().f + r->get_value().f };
	case ArithmeticOp::Subtraction:
		return NodeResult{ .f = l->get_value().f - r->get_value().f };
	case ArithmeticOp::Multiplication:
		return NodeResult{ .f = l->get_value().f * r->get_value().f };
	case ArithmeticOp::Division:
		return NodeResult{ .f = l->get_value().f / r->get_value().f };
	case ArithmeticOp::Power:
		return NodeResult{ .f = std::pow(l->get_value().f, r->get_value().f) };
	}

	std::unreachable();
}

std::size_t ArithmeticNode::node_id() const {
	return NODE_ID<ArithmeticNode>;
}

void ArithmeticNode::serialize(nlohmann::json& output_json) const
{
	output_json["operation"] = operation;

	output_json["lhs"] = lhs;
	output_json["rhs"] = rhs;
	output_json["output"] = output;
}

ArithmeticNode* ArithmeticNode::deserialize(NodeCircuit* parent, const nlohmann::json& input_json)
{
	return new ArithmeticNode{
		parent,
		input_json["operation"],

		input_json["lhs"],
		input_json["rhs"],
		input_json["output"]
	};
}
