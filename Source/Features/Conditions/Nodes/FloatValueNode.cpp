#include "FloatValueNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../Nodes.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cstddef>

FloatValueNode::FloatValueNode(NodeCircuit* parent, IdType output)
	: Node(parent, NodeType::FLOAT)
	, output(output)
{
}

FloatValueNode* FloatValueNode::initialized(NodeCircuit* parent)
{
	return new FloatValueNode{ parent, parent->next_id() };
}

FloatValueNode* FloatValueNode::uninitialized(NodeCircuit* parent)
{
	return new FloatValueNode{ parent, 0 };
}

void FloatValueNode::render_io()
{

	ImGui::SetNextItemWidth(100.0F);
	ImGui::DragFloat("Value", &value);

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

std::size_t FloatValueNode::node_id() const
{
	return NODE_ID<FloatValueNode>;
}

void FloatValueNode::serialize(nlohmann::json& output_json) const
{
	output_json["value"] = value;
	output_json["output"] = output;
}

void FloatValueNode::deserialize(const nlohmann::json& input_json)
{
	value = input_json["value"];
	output = input_json["output"];
}
