#include "NodeRegistry.hpp"

#include "IdType.hpp"
#include "NodeCircuit.hpp"

#include "Nodes/ArithmeticNode.hpp"
#include "Nodes/BooleanValueNode.hpp"
#include "Nodes/ComparisonNode.hpp"
#include "Nodes/FloatValueNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "magic_enum/magic_enum.hpp"

#include <cstddef>
#include <functional>
#include <string>
#include <utility>

NodeRegistry::NodeRegistry(NodeCircuit* parent)
	: parent(parent)
{

	menu["Primitives/Boolean value"] = [](NodeCircuit* parent) { return new BooleanValueNode{ parent }; };
	menu["Primitives/Float value"] = [](NodeCircuit* parent) { return new FloatValueNode{ parent }; };

	for (const ArithmeticOp op : magic_enum::enum_values<ArithmeticOp>())
		menu["Arithmetic/" + std::string{ magic_enum::enum_name(op) }]
			= [op](NodeCircuit* parent) { return new ArithmeticNode{ parent, op }; };

	for (const ComparisonOp op : magic_enum::enum_values<ComparisonOp>())
		menu["Comparison/" + replace_underscores_with_spaces(std::string{ magic_enum::enum_name(op) })]
			= [op](NodeCircuit* parent) { return new ComparisonNode{ parent, op }; };
}

void NodeRegistry::render_menu() const
{
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		ImGui::OpenPopup("New nodes");

	if (ImGui::BeginPopup("New nodes")) {
		const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
		Node* node = nullptr;

		// TODO clean this ugly thing up:
		for (const auto& [item_name, factory] : menu) {
			std::size_t p = 0;
			std::size_t depth = 0;
			std::size_t end = 0;
			bool aborted = false;
			while (true) {
				end = item_name.find('/', p);
				if (end == std::string::npos)
					break;
				if (!ImGui::BeginMenu(item_name.substr(p, end - p).c_str())) {
					aborted = true;
					break;
				}
				depth++;
				p = end + 1;
			}
			if (aborted) {
				// NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
				goto next_elem;
			}
			if (ImGui::MenuItem(item_name.substr(p).c_str()))
				node = factory(parent);

		next_elem:
			for (std::size_t i = 0; i < depth; i++)
				ImGui::EndMenu();
		}

		if (node) {
			const IdType id = parent->next_id();
			ImNodes::SetNodeScreenSpacePos(id, click_pos);
			parent->push_node(id, node);
		}
		ImGui::EndPopup();
	}
}

void NodeRegistry::add_node_type(std::string name, std::function<Node*(NodeCircuit*)> factory)
{
	menu[std::move(name)] = std::move(factory);
}
