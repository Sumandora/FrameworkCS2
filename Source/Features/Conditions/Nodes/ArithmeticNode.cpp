#include "ArithmeticNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "magic_enum/magic_enum.hpp"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <utility>

ArithmeticNode::ArithmeticNode(NodeCircuit* parent, ArithmeticOp operation, IdType lhs, IdType rhs, IdType output)
	: Node(parent)
	, operation(operation)
	, lhs(lhs)
	, rhs(rhs)
	, output(output)
{
}

ArithmeticNode* ArithmeticNode::initialized(NodeCircuit* parent, ArithmeticOp operation)
{
	return new ArithmeticNode{ parent, operation, parent->next_id(), parent->next_id(), parent->next_id() };
}

ArithmeticNode* ArithmeticNode::uninitialized(NodeCircuit* parent)
{
	return new ArithmeticNode{ parent, magic_enum::enum_values<ArithmeticOp>()[0], 0, 0, 0 };
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

void ArithmeticNode::serialize(nlohmann::json& output_json) const
{
	output_json["operation"] = operation;

	output_json["lhs"] = lhs;
	output_json["rhs"] = rhs;
	output_json["output"] = output;
}

void ArithmeticNode::deserialize(const nlohmann::json& input_json)
{
	operation = input_json["operation"];

	lhs = input_json["lhs"];
	rhs = input_json["rhs"];
	output = input_json["output"];
}
