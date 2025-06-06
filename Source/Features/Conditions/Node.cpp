#include "Node.hpp"

#include "IdType.hpp"
#include "imgui.h"
#include "imnodes.h"

#include "NodeCircuit.hpp"
#include "NodeType.hpp"

#include <string>

Node::Node(NodeCircuit* parent, NodeType output_type)
	: parent(parent)
	, output_type(output_type)
{
}

void Node::render_node(IdType id)
{
	ImNodes::BeginNode(id);

	if (queued_position.has_value()) {
		ImNodes::SetNodeGridSpacePos(id, queued_position.value());
		queued_position.reset();
	}

	ImNodes::BeginNodeTitleBar();
	std::string top_level_name = name;
	top_level_name = top_level_name.substr(top_level_name.find_last_of('/') + 1);
	ImGui::TextUnformatted(top_level_name.c_str());
	ImNodes::EndNodeTitleBar();

	render_io();

	ImNodes::EndNode();
}
