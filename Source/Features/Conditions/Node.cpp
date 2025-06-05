#include "Node.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "../../Utils/Logging.hpp"

#include "NodeType.hpp"
#include "NodeCircuit.hpp"

#include <string>
#include <utility>
 
Node::Node(NodeCircuit* parent, std::string name, NodeType output_type)
	: name(std::move(name))
	, parent(parent)
	, id(parent->next_id())
	, output_type(output_type)
{
	Logging::debug("Adding {} to ids with id {}: ptr {}", this->name, id, this);
	parent->push_node(id, this);
}


void Node::render_node()
{
	ImNodes::BeginNode(id);

	ImNodes::BeginNodeTitleBar();
	ImGui::TextUnformatted(name.c_str());
	ImNodes::EndNodeTitleBar();

	render_io();

	ImNodes::EndNode();
}
