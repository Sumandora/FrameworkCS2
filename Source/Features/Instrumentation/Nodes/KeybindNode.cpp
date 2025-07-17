#include "KeybindNode.hpp"

#include "../../../GUI/InputManager.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cstddef>
#include <memory>

KeybindNode::KeybindNode(NodeCircuit* parent, IdType output)
	: Node(parent)
	, key(std::make_shared<GUI::InputManager::Key>())
	, output(output)
{
	GUI::get_input_manager().register_key(key);
}

KeybindNode* KeybindNode::initialized(NodeCircuit* parent)
{
	return new KeybindNode{ parent, parent->next_id() };
}

KeybindNode* KeybindNode::uninitialized(NodeCircuit* parent)
{
	return new KeybindNode{ parent, 0 };
}

void KeybindNode::render_io()
{
	if (ImGui::BeginChild("##Key", { 100.0F, 0.0F }, ImGuiChildFlags_AutoResizeY))
		key->draw();
	ImGui::EndChild();

	ImGui::Spacing();

	ImNodes::BeginOutputAttribute(output);
	ImGui::TextUnformatted("output");
	ImNodes::EndOutputAttribute();
}

NodeResult KeybindNode::get_value(IdType /*id*/) const
{
	return key->is_active();
}

void KeybindNode::serialize(nlohmann::json& output_json) const
{
	output_json["output"] = output;
}

void KeybindNode::deserialize(const nlohmann::json& input_json)
{
	output = input_json["output"];
}
