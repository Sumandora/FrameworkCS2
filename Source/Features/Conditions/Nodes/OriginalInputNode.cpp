#include "OriginalInputNode.hpp"

#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cstddef>
#include <functional>
#include <utility>

OriginalInputNode::OriginalInputNode(NodeCircuit* parent, NodeType type, std::function<NodeResult()> get_original_value)
	: Node(parent)
	, get_original_value(std::move(get_original_value))
	, output(parent->next_id())
	, type(type)
{
}

void OriginalInputNode::render_io()
{
	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
};

NodeResult OriginalInputNode::get_value() const
{
	return get_original_value();
}

void OriginalInputNode::serialize(nlohmann::json& /*output_json*/) const
{
	std::unreachable();
}

void OriginalInputNode::deserialize(const nlohmann::json& /*input_json*/)
{
	std::unreachable();
}
