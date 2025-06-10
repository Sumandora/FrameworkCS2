#include "NodeRegistry.hpp"

#include "IdType.hpp"
#include "NodeCircuit.hpp"

#include "Nodes/ArithmeticNode.hpp"
#include "Nodes/BooleanValueNode.hpp"
#include "Nodes/ComparisonNode.hpp"
#include "Nodes/FloatValueNode.hpp"
#include "Nodes/HSVNode.hpp"
#include "Nodes/LogicGateNode.hpp"
#include "Nodes/NotGateNode.hpp"
#include "Nodes/RGBANode.hpp"

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

	menu["Primitives/Boolean value"] = {
		.create_initialized = BooleanValueNode::initialized,
		.create_uninitialized = BooleanValueNode::uninitialized,
	};
	menu["Primitives/Float value"] = {
		.create_initialized = FloatValueNode::initialized,
		.create_uninitialized = FloatValueNode::uninitialized,
	};

	for (const ArithmeticOp op : magic_enum::enum_values<ArithmeticOp>())
		menu["Arithmetic/" + std::string{ magic_enum::enum_name(op) }] = {
			.create_initialized = [op](NodeCircuit* parent) { return ArithmeticNode::initialized(parent, op); },
			.create_uninitialized = ArithmeticNode::uninitialized,
		};

	for (const ComparisonOp op : magic_enum::enum_values<ComparisonOp>())
		menu["Comparison/" + replace_underscores_with_spaces(std::string{ magic_enum::enum_name(op) })] = {
			.create_initialized = [op](NodeCircuit* parent) { return ComparisonNode::initialized(parent, op); },
			.create_uninitialized = [op](NodeCircuit* parent) { return ComparisonNode::uninitialized(parent, op); },
		};

	menu["Color conversion/RGBA to Color"] = {
		.create_initialized = [](NodeCircuit* parent) { return RGBANode::initialized(parent, RGBADirection::RGBA_TO_COLOR); },
		.create_uninitialized = RGBANode::uninitialized
	};
	menu["Color conversion/Color to RGBA"] = {
		.create_initialized = [](NodeCircuit* parent) { return RGBANode::initialized(parent, RGBADirection::COLOR_TO_RGBA); },
		.create_uninitialized = RGBANode::uninitialized
	};

	menu["Color conversion/HSVA to Color"] = {
		.create_initialized = [](NodeCircuit* parent) { return HSVNode::initialized(parent, HSVDirection::HSVA_TO_COLOR); },
		.create_uninitialized = HSVNode::uninitialized
	};
	menu["Color conversion/Color to HSVA"] = {
		.create_initialized = [](NodeCircuit* parent) { return HSVNode::initialized(parent, HSVDirection::COLOR_TO_HSVA); },
		.create_uninitialized = HSVNode::uninitialized
	};

	menu["Logic Gate/And gate"] = {
		.create_initialized = [](NodeCircuit* parent) { return LogicGateNode::initialized(parent, LogicGateType::And); },
		.create_uninitialized = LogicGateNode::uninitialized
	};
	menu["Logic Gate/Or gate"] = {
		.create_initialized = [](NodeCircuit* parent) { return LogicGateNode::initialized(parent, LogicGateType::Or); },
		.create_uninitialized = LogicGateNode::uninitialized
	};
	menu["Logic Gate/Not gate"] = {
		.create_initialized = NotGateNode::initialized,
		.create_uninitialized = NotGateNode::uninitialized
	};
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
			if (ImGui::MenuItem(item_name.substr(p).c_str())) {
				node = factory.create_initialized(parent);
				node->set_name(item_name);
			}
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

Node* NodeRegistry::create_by_name(const std::string& name) const
{
	Node* node = menu.at(name).create_uninitialized(parent);
	node->set_name(name);
	return node;
}

void NodeRegistry::add_node_type(std::string name, FactoryPair factory)
{
	menu[std::move(name)] = std::move(factory);
}
