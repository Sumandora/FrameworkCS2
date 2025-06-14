#include "NodeCircuit.hpp"

#include "IdType.hpp"
#include "Node.hpp"
#include "NodeRegistry.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "Nodes/OriginalInputNode.hpp"
#include "Nodes/OutputNode.hpp"

#include "../../Utils/Logging.hpp"

#include "../../GUI/Theme.hpp"

#include "../../Notifications/Notifications.hpp"

#include "magic_enum/magic_enum.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include "imnodes_internal.h"

#include "nlohmann/json_fwd.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <functional>
#include <string>
#include <utility>
#include <vector>

bool NodeCircuit::is_dynamic_node(Node* node) const
{
	return node != &original_input_node && node != &output_node;
}

NodeCircuit::NodeCircuit(NodeType type, std::function<NodeResult()> get_original_value)
	: imnodes_context(ImNodes::CreateContext())
	, original_input_node(this, type, std::move(get_original_value))
	, original_input_node_id(next_id())
	, output_node(this, type)
	, output_node_id(next_id())
{
	GUI::Theme::anti_purple_imnodes_theme();

	ImNodesIO& io = ImNodes::GetIO();
	io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;

	ImNodes::SetNodeScreenSpacePos(original_input_node_id, ImVec2{ 100, 100 });
	ImNodes::SetNodeScreenSpacePos(output_node_id, ImVec2{ 300, 100 });

	original_input_node.set_name("Original input");
	output_node.set_name("Output");

	push_node(original_input_node_id, &original_input_node);
	push_node(output_node_id, &output_node);

	links.emplace_back(Link{
		.start_node = original_input_node_id,
		.start_attribute = original_input_node.get_output(),
		.end_node = output_node_id,
		.end_attribute = output_node.get_input() });
}

NodeCircuit::~NodeCircuit()
{
	ImNodes::DestroyContext(imnodes_context);
	for (const auto& [id, node] : ids) {
		if (is_dynamic_node(node))
			delete node;
	}
}

void NodeCircuit::push_node(IdType id, Node* node)
{
	Logging::debug("Adding {} to ids with id {}: ptr {}", node->get_name(), id, this);
	ids[id] = node;
}

void NodeCircuit::render(bool newly_opened)
{
	ImNodes::SetCurrentContext(imnodes_context);
	ImNodes::BeginNodeEditor();

	registry.render_menu();

	for (auto& [id, node] : ids)
		node->render_node(id);

	for (std::size_t i = 0; i < links.size(); i++) {
		const Link& p = links[i];
		ImNodes::Link(static_cast<IdType>(i), p.start_attribute, p.end_attribute);
	}

	ImNodes::MiniMap(0.1F, ImNodesMiniMapLocation_BottomRight);

	ImNodes::EndNodeEditor();

	if (newly_opened)
		center_editor();

	Link link{};
	if (ImNodes::IsLinkCreated(&link.start_node, &link.start_attribute, &link.end_node, &link.end_attribute)) {
		Node* start = ids[link.start_node];
		Node* end = ids[link.end_node];

		const NodeType from_type = start->get_output_type(link.start_attribute);
		const NodeType to_type = end->get_input_type(link.end_attribute);

		if (std::ranges::any_of(links, [link](const Link& other_link) { return link.end_attribute == other_link.end_attribute; }))
			Notifications::create("Node circuit", "Attempted to connect 2 values to a single input.", Notifications::Severity::ERROR);
		else if (from_type == NodeType::ANYTHING || to_type == NodeType::ANYTHING || from_type == to_type)
			links.emplace_back(link);
		else
			Notifications::create("Node circuit",
				std::format("Type mismatch! Attempted to connect node of type '{}' to node of type '{}'.",
					magic_enum::enum_name(from_type),
					magic_enum::enum_name(to_type)),
				Notifications::Severity::ERROR);
	}

	IdType link_id = 0;
	if (ImNodes::IsLinkDestroyed(&link_id))
		std::erase_if(links, [link_id](const Link& link) { return link.start_attribute == link_id || link.end_attribute == link_id; });
	if (ImNodes::IsLinkDropped(&link_id))
		std::erase_if(links, [link_id](const Link& link) { return link.start_attribute == link_id || link.end_attribute == link_id; });

	const int num_selected = ImNodes::NumSelectedNodes();
	if (num_selected > 0 && ImGui::IsKeyReleased(ImGuiKey_Delete)) {
		static std::vector<IdType> selected_nodes;
		selected_nodes.resize(static_cast<size_t>(num_selected));
		ImNodes::GetSelectedNodes(selected_nodes.data());
		for (const IdType node_id : selected_nodes) {
			if (node_id == output_node_id)
				continue;
			if (node_id == original_input_node_id)
				continue;
			std::erase_if(ids, [node_id](const auto& pair) { return pair.first == node_id; });
			std::erase_if(links, [node_id](const Link& link) { return link.start_node == node_id || link.end_node == node_id; });
		}
	}

	if (num_selected == 0 && ImGui::IsKeyReleased(ImGuiKey_Delete) && ImNodes::IsLinkHovered(&link_id))
		std::erase_if(links, [link_id](const Link& link) { return link.start_attribute == link_id || link.end_attribute == link_id; });
	ImNodes::SetCurrentContext(nullptr);
}

inline ImVec2 ScreenSpaceToGridSpace(const ImNodesEditorContext& editor, const ImVec2& v)
{
	return v - GImNodes->CanvasOriginScreenSpace - editor.Panning;
}

inline ImVec2 ScreenSpaceToMiniMapSpace(const ImNodesEditorContext& editor, const ImVec2& v)
{
	return (ScreenSpaceToGridSpace(editor, v) - editor.GridContentBounds.Min) * editor.MiniMapScaling + editor.MiniMapContentScreenSpace.Min;
};

void NodeCircuit::center_editor()
{
	// TODO reduce this code,
	// this is stolen from the minimap panning code
	ImVec2 nodes_center;
	std::size_t count = 0;

	ImNodesEditorContext& editor = ImNodes::EditorContextGet();
	for (int node_idx = 0; node_idx < editor.Nodes.Pool.size(); ++node_idx) {
		if (editor.Nodes.InUse[node_idx]) {
			const ImNodeData& node_data = editor.Nodes.Pool[node_idx];
			nodes_center += ScreenSpaceToMiniMapSpace(editor, node_data.Rect.GetCenter());
			count++;
		}
	}

	const ImVec2 target = ((nodes_center / static_cast<float>(count)) - editor.MiniMapContentScreenSpace.Min) / editor.MiniMapScaling + editor.GridContentBounds.Min;
	const ImVec2 center = GImNodes->CanvasRectScreenSpace.GetSize() * 0.5F;
	editor.Panning = ImFloor(center - target);
	Logging::debug("Panned to {}x{}", editor.Panning.x, editor.Panning.y);
}

bool NodeCircuit::is_trivial() const
{
	// There are only two ids: Output and Original input, we don't even need to check for that, since they can't be removed.
	// If there is either no link between them: Great, you are a moron and you managed to break the setting.
	// If there is a single link between them: Then you haven't changed anything.
	return ids.size() <= 2 && links.size() <= 1;
}

Node* NodeCircuit::node_from_start_attrib(IdType id) const
{
	auto it = std::ranges::find_if(links, [id](const Link& link) { return link.start_attribute == id; });
	if (it != links.end())
		return ids.at(it->end_node);
	return nullptr;
}

Node* NodeCircuit::node_from_end_attrib(IdType id) const
{
	auto it = std::ranges::find_if(links, [id](const Link& link) { return link.end_attribute == id; });
	if (it != links.end())
		return ids.at(it->start_node);
	return nullptr;
}

NodeResult NodeCircuit::value_from_attribute(IdType id) const
{
	auto it = std::ranges::find_if(links, [id](const Link& link) { return link.end_attribute == id; });
	if (it != links.end())
		return ids.at(it->start_node)->get_value(it->start_attribute);
	return {};
}

void NodeCircuit::serialize(nlohmann::json& output_json) const
{
	ImNodes::SetCurrentContext(imnodes_context);
	output_json["Id counter"] = id_counter;

	auto& nodes = output_json["Nodes"];
	for (const auto& [id, node] : ids) {
		auto& node_json = nodes.emplace_back();
		ImVec2 v = ImNodes::GetNodeGridSpacePos(id);
		node_json["Position"] = { v.x, v.y };
		node_json["Node name"] = node->get_name();
		node_json["Id"] = id;
		if (!is_dynamic_node(node))
			continue;
		node->serialize(node_json["Inner"]);
	}

	output_json["Links"] = links;
	ImNodes::SetCurrentContext(nullptr);
}

void NodeCircuit::deserialize(const nlohmann::json& input_json)
{
	std::erase_if(ids, [this](const std::pair<IdType, Node*>& pair) {
		if (is_dynamic_node(pair.second)) {
			delete pair.second;
			return true;
		}
		return false;
	});

	links.clear();

	const auto& nodes = input_json["Nodes"];
	for (const auto& node : nodes) {
		if (!node.contains("Inner")) // Don't try to load static nodes.
			continue;

		const std::string node_name = node["Node name"];
		const IdType id = node["Id"];
		Node* new_node = registry.create_by_name(node_name);
		new_node->deserialize(node["Inner"]);
		push_node(id, new_node);
	}

	links = input_json["Links"];

	id_counter = input_json["Id counter"];

	ImNodes::DestroyContext(imnodes_context);
	ImNodes::CreateContext();
	imnodes_context = ImNodes::GetCurrentContext();

	for (const auto& node : nodes) {
		const IdType id = node["Id"];
		const ImVec2 v = {
			node["Position"][0],
			node["Position"][1],
		};
		Logging::debug("Positioning {} at {}x{}", id, v.x, v.y);
		ids[id]->queue_position(v);
	}

	ImNodes::SetCurrentContext(nullptr);
}
