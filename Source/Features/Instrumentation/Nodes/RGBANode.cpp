#include "RGBANode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "nlohmann/json.hpp"

#include <algorithm>
#include <initializer_list>
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
RGBANode* RGBANode::uninitialized(NodeCircuit* parent, RGBADirection direction)
{
	return new RGBANode{ parent, direction, 0, 0, 0, 0, 0 };
}

void RGBANode::render_io()
{
	switch (direction) {
	case RGBADirection::RGBA_TO_COLOR:
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
	case RGBADirection::COLOR_TO_RGBA:
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

NodeResult RGBANode::get_value(IdType id) const
{
	switch (direction) {
	case RGBADirection::RGBA_TO_COLOR: {
		const NodeResult r = get_parent()->value_from_attribute(this->r);
		const NodeResult g = get_parent()->value_from_attribute(this->g);
		const NodeResult b = get_parent()->value_from_attribute(this->b);
		const NodeResult a = get_parent()->value_from_attribute(this->a);

		if (std::ranges::any_of(std::initializer_list{ r, g, b, a }, [](const NodeResult& nr) { return nr.empty(); }))
			return {};

		return ImColor(
			r.get<float>(),
			g.get<float>(),
			b.get<float>(),
			a.get<float>());
	}
	case RGBADirection::COLOR_TO_RGBA:
		const NodeResult color = get_parent()->value_from_attribute(this->color);

		const auto col = color.get<ImColor>();

		if (id == r)
			return col.Value.x;
		if (id == g)
			return col.Value.y;
		if (id == b)
			return col.Value.z;
		if (id == a)
			return col.Value.w;
	}

	std::unreachable();
}

void RGBANode::serialize(nlohmann::json& output_json) const
{
	output_json["r"] = r;
	output_json["g"] = g;
	output_json["b"] = b;
	output_json["a"] = a;
	output_json["color"] = color;
}

void RGBANode::deserialize(const nlohmann::json& input_json)
{
	r = input_json["r"];
	g = input_json["g"];
	b = input_json["b"];
	a = input_json["a"];
	color = input_json["color"];
}
