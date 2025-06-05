#include "ArithmeticNode.hpp"

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

ArithmeticNode::ArithmeticNode(NodeCircuit* parent, ArithmeticOp operation)
	: Node(parent, std::string{ magic_enum::enum_name(operation) }, NodeType::FLOAT)
	, lhs(parent->next_id())
	, rhs(parent->next_id())
	, output(parent->next_id())
	, operation(operation)
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
