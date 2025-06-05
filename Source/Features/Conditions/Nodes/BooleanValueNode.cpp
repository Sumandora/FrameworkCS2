#include "BooleanValueNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

BooleanValueNode::BooleanValueNode(NodeCircuit* parent)
	: Node(parent, "Boolean value", NodeType::BOOLEAN)
	, output(parent->next_id())
{
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
