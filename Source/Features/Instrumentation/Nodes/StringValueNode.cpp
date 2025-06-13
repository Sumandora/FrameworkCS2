#include "StringValueNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"

#include "imgui.h"
#include "imnodes.h"
#include "misc/cpp/imgui_stdlib.h"

#include <cstddef>

StringValueNode::StringValueNode(NodeCircuit* parent, IdType output)
	: Node(parent)
	, output(output)
{
}

StringValueNode* StringValueNode::initialized(NodeCircuit* parent)
{
	return new StringValueNode{ parent, parent->next_id() };
}

StringValueNode* StringValueNode::uninitialized(NodeCircuit* parent)
{
	return new StringValueNode{ parent, 0 };
}

void StringValueNode::render_io()
{
	ImGui::SetNextItemWidth(100.0F);
	ImGui::InputText("Value", &value);

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

void StringValueNode::serialize(nlohmann::json& output_json) const
{
	output_json["value"] = value;
	output_json["output"] = output;
}

void StringValueNode::deserialize(const nlohmann::json& input_json)
{
	value = input_json["value"];
	output = input_json["output"];
}
