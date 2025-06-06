#include "BooleanValueNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../Nodes.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cstddef>

BooleanValueNode::BooleanValueNode(NodeCircuit* parent, IdType output)
	: Node(parent, NodeType::BOOLEAN)
	, output(output)
{
}

BooleanValueNode* BooleanValueNode::initialized(NodeCircuit* parent)
{
	return new BooleanValueNode{ parent, parent->next_id() };
}

BooleanValueNode* BooleanValueNode::uninitialized(NodeCircuit* parent)
{
	return new BooleanValueNode{ parent, 0 };
}

void BooleanValueNode::render_io()
{

	ImGui::SetNextItemWidth(100.0F);
	int temp = value ? 1 : 0;
	ImGui::Combo("Value", &temp, "False\0True\0");
	value = temp != 0;

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

std::size_t BooleanValueNode::node_id() const
{
	return NODE_ID<BooleanValueNode>;
}

void BooleanValueNode::serialize(nlohmann::json& output_json) const
{
	output_json["value"] = value;
	output_json["output"] = output;
}

void BooleanValueNode::deserialize(const nlohmann::json& input_json)
{
	value = input_json["value"];
	output = input_json["output"];
}
