#include "FloatValueNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

FloatValueNode::FloatValueNode(NodeCircuit* parent)
	: Node(parent, "Float value", NodeType::FLOAT)
	, output(parent->next_id())
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
