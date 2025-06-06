#include "Node.hpp"

#include "IdType.hpp"
#include "imgui.h"
#include "imnodes.h"

#include "NodeCircuit.hpp"
#include "NodeType.hpp"

#include <string>
#include <utility>

Node::Node(NodeCircuit* parent, std::string name, NodeType output_type)
	: name(std::move(name))
	, parent(parent)
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
	ImGui::TextUnformatted(name.c_str());
	ImNodes::EndNodeTitleBar();

	render_io();

	ImNodes::EndNode();
}
