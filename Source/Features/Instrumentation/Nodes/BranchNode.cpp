#include "BranchNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cassert>

BranchNode::BranchNode(NodeCircuit* parent, IdType falsy, IdType condition, IdType truthy, IdType output)
	: Node(parent)
	, falsy(falsy)
	, condition(condition)
	, truthy(truthy)
	, output(output)
{
}

BranchNode* BranchNode::initialized(NodeCircuit* parent)
{
	return new BranchNode{ parent, parent->next_id(), parent->next_id(), parent->next_id(), parent->next_id() };
}
BranchNode* BranchNode::uninitialized(NodeCircuit* parent)
{
	return new BranchNode{ parent, 0, 0, 0, 0 };
}

void BranchNode::render_io()
{
	ImNodes::BeginInputAttribute(falsy);
	ImGui::TextUnformatted("falsy");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(condition);
	ImGui::TextUnformatted("condition");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(truthy);
	ImGui::TextUnformatted("truthy");
	ImNodes::EndInputAttribute();

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

NodeResult BranchNode::get_value(IdType /*id*/) const
{
	const NodeResult l_or_r = get_parent()->value_from_attribute(condition);

	if (l_or_r.empty())
		return {};

	const NodeResult falsy = get_parent()->value_from_attribute(this->falsy);
	const NodeResult truthy = get_parent()->value_from_attribute(this->truthy);

	if (falsy.empty() || truthy.empty())
		return {};

	return l_or_r.get<bool>() ? truthy : falsy;
}

NodeType BranchNode::get_input_type(IdType id) const
{
	if (id == condition)
		return NodeType::BOOLEAN;

	const Node* falsy = get_parent()->node_from_end_attrib(this->falsy);
	const Node* truthy = get_parent()->node_from_end_attrib(this->truthy);
	const Node* output = get_parent()->node_from_start_attrib(this->output);

	if (falsy)
		return falsy->get_output_type(this->falsy);
	if (truthy)
		return truthy->get_output_type(this->truthy);
	if (output)
		return output->get_input_type(this->output);

	return NodeType::ANYTHING;
}

NodeType BranchNode::get_output_type(IdType /*id*/) const
{
	const Node* falsy = get_parent()->node_from_end_attrib(this->falsy);
	const Node* truthy = get_parent()->node_from_end_attrib(this->truthy);
	// No need to check the output here, since output_type only matters when currently connecting something to the output

	if (falsy)
		return falsy->get_output_type(this->falsy);
	if (truthy)
		return truthy->get_output_type(this->truthy);

	return NodeType::ANYTHING;
}

void BranchNode::serialize(nlohmann::json& output_json) const
{
	output_json["falsy"] = falsy;
	output_json["condition"] = condition;
	output_json["truthy"] = truthy;
	output_json["output"] = output;
}

void BranchNode::deserialize(const nlohmann::json& input_json)
{
	falsy = input_json["falsy"];
	condition = input_json["condition"];
	truthy = input_json["truthy"];
	output = input_json["output"];
}
