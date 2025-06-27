#include "NotGateNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

NotGateNode::NotGateNode(NodeCircuit* parent, IdType input, IdType output)
	: Node(parent)
	, input(input)
	, output(output)
{
}

NotGateNode* NotGateNode::initialized(NodeCircuit* parent)
{
	return new NotGateNode{ parent, parent->next_id(), parent->next_id() };
}
NotGateNode* NotGateNode::uninitialized(NodeCircuit* parent)
{
	return new NotGateNode{ parent, 0, 0 };
}

void NotGateNode::render_io()
{
	ImNodes::BeginInputAttribute(input);
	ImGui::TextUnformatted("Input");
	ImNodes::EndInputAttribute();

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

NodeResult NotGateNode::get_value(IdType /*id*/) const
{
	const NodeResult input = get_parent()->value_from_attribute(this->input);

	if (input.empty())
		return {};

	return !input.get<bool>();
}

void NotGateNode::serialize(nlohmann::json& output_json) const
{
	output_json["input"] = input;
	output_json["output"] = output;
}

void NotGateNode::deserialize(const nlohmann::json& input_json)
{
	input = input_json["input"];
	output = input_json["output"];
}
