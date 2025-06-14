#include "LogicGateNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <utility>

LogicGateNode::LogicGateNode(NodeCircuit* parent, LogicGateType type, IdType input_a, IdType input_b, IdType output)
	: Node(parent)
	, type(type)
	, input_a(input_a)
	, input_b(input_b)
	, output(output)
{
}

LogicGateNode* LogicGateNode::initialized(NodeCircuit* parent, LogicGateType type)
{
	return new LogicGateNode{ parent, type, parent->next_id(), parent->next_id(), parent->next_id() };
}
LogicGateNode* LogicGateNode::uninitialized(NodeCircuit* parent, LogicGateType type)
{
	return new LogicGateNode{ parent, type, 0, 0, 0 };
}

void LogicGateNode::render_io()
{
	ImNodes::BeginInputAttribute(input_a);
	ImGui::TextUnformatted("Input A");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(input_b);
	ImGui::TextUnformatted("Input B");
	ImNodes::EndInputAttribute();

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

NodeResult LogicGateNode::get_value(IdType /*id*/) const
{
	const NodeResult a = get_parent()->value_from_attribute(input_a);
	const NodeResult b = get_parent()->value_from_attribute(input_b);

	if(a.empty() || b.empty())
		return {};

	const auto input_a = a.get<bool>();
	const auto input_b = b.get<bool>();

	switch (type) {
	case LogicGateType::And:
		return input_a && input_b;
	case LogicGateType::Or:
		return input_a || input_b;
	}

	std::unreachable();
}

void LogicGateNode::serialize(nlohmann::json& output_json) const
{
	output_json["input_a"] = input_a;
	output_json["input_b"] = input_b;
	output_json["output"] = output;	
}

void LogicGateNode::deserialize(const nlohmann::json& input_json)
{
	input_a = input_json["input_a"];
	input_b = input_json["input_b"];
	output = input_json["output"];
}
