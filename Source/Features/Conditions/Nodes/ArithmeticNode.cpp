#include "ArithmeticNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cctype>
#include <cmath>
#include <cstddef>
#include <optional>
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

ArithmeticNode* ArithmeticNode::uninitialized(NodeCircuit* parent, ArithmeticOp operation)
{
	return new ArithmeticNode{ parent, operation, 0, 0, 0 };
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

NodeResult ArithmeticNode::get_value(IdType /*id*/) const
{
	const NodeResult l = get_parent()->value_from_attribute(lhs);
	const NodeResult r = get_parent()->value_from_attribute(rhs);

	if (l.empty() || r.empty())
		return {};

	const auto lhs = l.get<float>();
	const auto rhs = r.get<float>();

	switch (operation) {
	case ArithmeticOp::Addition:
		return lhs + rhs;
	case ArithmeticOp::Subtraction:
		return lhs - rhs;
	case ArithmeticOp::Multiplication:
		return lhs * rhs;
	case ArithmeticOp::Division:
		return lhs / rhs;
	case ArithmeticOp::Power:
		return std::pow(lhs, rhs);
	}

	std::unreachable();
}

void ArithmeticNode::serialize(nlohmann::json& output_json) const
{
	output_json["lhs"] = lhs;
	output_json["rhs"] = rhs;
	output_json["output"] = output;
}

void ArithmeticNode::deserialize(const nlohmann::json& input_json)
{
	lhs = input_json["lhs"];
	rhs = input_json["rhs"];
	output = input_json["output"];
}
