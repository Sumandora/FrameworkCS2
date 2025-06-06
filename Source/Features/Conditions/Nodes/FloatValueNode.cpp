#include "FloatValueNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../Nodes.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cstddef>

FloatValueNode::FloatValueNode(NodeCircuit* parent, IdType output)
	: Node(parent, "Float value", NodeType::FLOAT)
	, output(output)
{
}

FloatValueNode::FloatValueNode(NodeCircuit* parent)
	: FloatValueNode(parent, parent->next_id())
{
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

FloatValueNode* FloatValueNode::deserialize(NodeCircuit* parent, const nlohmann::json& input_json)
{
	auto* node = new FloatValueNode{
		parent,
		input_json["output"]
	};
	node->value = input_json["value"];
	return node;
}
