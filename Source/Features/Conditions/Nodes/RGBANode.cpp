#include "RGBANode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "magic_enum/magic_enum.hpp"
#include "nlohmann/json.hpp"

#include <utility>

RGBANode::RGBANode(NodeCircuit* parent, RGBADirection direction, IdType r, IdType g, IdType b, IdType a, IdType color)
	: Node(parent)
	, direction(direction)
	, r(r)
	, g(g)
	, b(b)
	, a(a)
	, color(color)
{
}

RGBANode* RGBANode::initialized(NodeCircuit* parent, RGBADirection direction)
{
	return new RGBANode{ parent, direction, parent->next_id(), parent->next_id(), parent->next_id(), parent->next_id(), parent->next_id() };
}
RGBANode* RGBANode::uninitialized(NodeCircuit* parent)
{
	return new RGBANode{ parent, magic_enum::enum_values<RGBADirection>()[0], 0, 0, 0, 0, 0 };
}

void RGBANode::render_io()
{
	switch (direction) {
	case RGBADirection::FLOATS_TO_RGBA:
		ImNodes::BeginInputAttribute(r);
		ImGui::TextUnformatted("red");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(g);
		ImGui::TextUnformatted("green");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(b);
		ImGui::TextUnformatted("blue");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(a);
		ImGui::TextUnformatted("alpha");
		ImNodes::EndInputAttribute();

		ImGui::Spacing();

		ImNodes::BeginOutputAttribute(color);
		ImGui::TextUnformatted("color");
		ImNodes::EndOutputAttribute();
		return;
	case RGBADirection::RGBA_TO_FLOATS:
		ImNodes::BeginInputAttribute(color);
		ImGui::TextUnformatted("color");
		ImNodes::EndInputAttribute();

		ImGui::Spacing();

		ImNodes::BeginOutputAttribute(r);
		ImGui::TextUnformatted("red");
		ImNodes::EndOutputAttribute();
		ImNodes::BeginOutputAttribute(g);
		ImGui::TextUnformatted("green");
		ImNodes::EndOutputAttribute();
		ImNodes::BeginOutputAttribute(b);
		ImGui::TextUnformatted("blue");
		ImNodes::EndOutputAttribute();
		ImNodes::BeginOutputAttribute(a);
		ImGui::TextUnformatted("alpha");
		ImNodes::EndOutputAttribute();
		return;
	}
}

NodeResult RGBANode::get_value() const
{
	switch (direction) {
	case RGBADirection::FLOATS_TO_RGBA: {
		const Node* r = get_parent()->from_id(this->r);
		const Node* g = get_parent()->from_id(this->g);
		const Node* b = get_parent()->from_id(this->b);
		const Node* a = get_parent()->from_id(this->a);

		return { .color = ImColor(
					 r ? r->get_value().f : 0.0F,
					 g ? g->get_value().f : 0.0F,
					 b ? b->get_value().f : 0.0F,
					 a ? a->get_value().f : 0.0F) };
	}
	case RGBADirection::RGBA_TO_FLOATS:
		std::unreachable(); // TODO
		return {};
	}
}

void RGBANode::serialize(nlohmann::json& output_json) const
{
	output_json["r"] = r;
	output_json["g"] = g;
	output_json["b"] = b;
	output_json["a"] = a;
	output_json["color"] = color;

	output_json["direction"] = direction;
}

void RGBANode::deserialize(const nlohmann::json& input_json)
{
	r = input_json["r"];
	g = input_json["g"];
	b = input_json["b"];
	a = input_json["a"];
	color = input_json["color"];

	direction = input_json["direction"];
}
