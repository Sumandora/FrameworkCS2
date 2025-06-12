#include "BranchNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cassert>
#include <optional>

BranchNode::BranchNode(NodeCircuit* parent, IdType truthy, IdType condition, IdType falsy, IdType output)
	: Node(parent)
	, truthy(truthy)
	, condition(condition)
	, falsy(falsy)
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
	ImNodes::BeginInputAttribute(truthy);
	ImGui::TextUnformatted("truthy");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(condition);
	ImGui::TextUnformatted("condition");
	ImNodes::EndInputAttribute();

	ImNodes::BeginInputAttribute(falsy);
	ImGui::TextUnformatted("falsy");
	ImNodes::EndInputAttribute();

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

NodeResult BranchNode::get_value(IdType /*id*/) const
{
	const std::optional<NodeResult> l_or_r = get_parent()->value_from_attribute(condition);

	if (!l_or_r.has_value())
		return {};

	const std::optional<NodeResult> truthy = get_parent()->value_from_attribute(this->truthy);
	const std::optional<NodeResult> falsy = get_parent()->value_from_attribute(this->falsy);

	if (!truthy.has_value() || !falsy.has_value())
		return {};

	return l_or_r->b ? truthy.value() : falsy.value();
}

NodeType BranchNode::get_input_type(IdType id) const
{
	if (id == condition)
		return NodeType::BOOLEAN;

	const Node* truthy = get_parent()->node_from_end_attrib(this->truthy);
	const Node* falsy = get_parent()->node_from_end_attrib(this->falsy);
	const Node* output = get_parent()->node_from_start_attrib(this->output);

	if (truthy)
		return truthy->get_output_type(this->truthy);
	if (falsy)
		return falsy->get_output_type(this->falsy);
	if (output)
		return output->get_input_type(this->output);

	return NodeType::ANYTHING;
}

NodeType BranchNode::get_output_type(IdType /*id*/) const
{
	const Node* truthy = get_parent()->node_from_end_attrib(this->truthy);
	const Node* falsy = get_parent()->node_from_end_attrib(this->falsy);
	// No need to check the output here, since output_type only matters when currently connecting something to the output

	if (truthy)
		return truthy->get_output_type(this->truthy);
	if (falsy)
		return falsy->get_output_type(this->falsy);

	return NodeType::ANYTHING;
}

void BranchNode::serialize(nlohmann::json& output_json) const
{
	output_json["truthy"] = truthy;
	output_json["condition"] = condition;
	output_json["falsy"] = falsy;
	output_json["output"] = output;
}

void BranchNode::deserialize(const nlohmann::json& input_json)
{
	truthy = input_json["truthy"];
	condition = input_json["condition"];
	falsy = input_json["falsy"];
	output = input_json["output"];
}
