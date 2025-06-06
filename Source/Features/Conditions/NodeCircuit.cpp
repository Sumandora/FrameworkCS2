#include "NodeCircuit.hpp"

#include "IdType.hpp"
#include "Node.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "../../Utils/Logging.hpp"

#include "imgui_internal.h"
#include "imnodes_internal.h"
#include "magic_enum/magic_enum.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <utility>
#include <vector>

NodeCircuit::NodeCircuit(NodeType type, std::function<NodeResult()> get_original_value)
	: imnodes_context(ImNodes::CreateContext())
	, original_input_node(this, type, std::move(get_original_value))
	, output_node(this, type)
{
	ImNodesIO& io = ImNodes::GetIO();
	io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;

	ImNodes::SetNodeScreenSpacePos(original_input_node.get_id(), ImVec2{ 100, 100 });
	ImNodes::SetNodeScreenSpacePos(output_node.get_id(), ImVec2{ 300, 100 });

	links.emplace_back(Link{
		.start_node = original_input_node.get_id(),
		.start_attribute = original_input_node.get_output(),
		.end_node = output_node.get_id(),
		.end_attribute = output_node.get_input() });
}

NodeCircuit::~NodeCircuit() {
	ImNodes::DestroyContext(imnodes_context);
}

void NodeCircuit::render(bool newly_opened)
{
	ImNodes::SetCurrentContext(imnodes_context);
	ImNodes::BeginNodeEditor();

	registry.render_menu();

	for (auto& [id, node] : ids)
		node->render_node();

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

		const NodeType from_type = start->get_output_type();
		const NodeType to_type = end->get_input_type(link.end_attribute);

		if (std::ranges::any_of(links, [link](const Link& other_link) { return link.end_attribute == other_link.end_attribute; }))
			Logging::error("Attempted to connect 2 values to a single input");
		else if (from_type == to_type)
			links.emplace_back(link);
		else
			Logging::error("Attempted to connect nodes of different types. ('{}' != '{}')", magic_enum::enum_name(from_type), magic_enum::enum_name(to_type));
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
			if (node_id == output_node.get_id())
				continue;
			if (node_id == original_input_node.get_id())
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

Node* NodeCircuit::from_id(IdType id) const
{
	auto it = std::ranges::find_if(links, [id](const Link& link) { return link.end_attribute == id; });
	if (it != links.end())
		return ids.at(it->start_node);
	return nullptr;
}
