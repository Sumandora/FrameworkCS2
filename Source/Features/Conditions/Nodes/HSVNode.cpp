#include "HSVNode.hpp"

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

HSVNode::HSVNode(NodeCircuit* parent, HSVDirection direction, IdType h, IdType s, IdType v, IdType a, IdType color)
	: Node(parent)
	, direction(direction)
	, h(h)
	, s(s)
	, v(v)
	, a(a)
	, color(color)
{
}

HSVNode* HSVNode::initialized(NodeCircuit* parent, HSVDirection direction)
{
	return new HSVNode{ parent, direction, parent->next_id(), parent->next_id(), parent->next_id(), parent->next_id(), parent->next_id() };
}
HSVNode* HSVNode::uninitialized(NodeCircuit* parent, HSVDirection direction)
{
	return new HSVNode{ parent, direction, 0, 0, 0, 0, 0 };
}

void HSVNode::render_io()
{
	switch (direction) {
	case HSVDirection::HSVA_TO_COLOR:
		ImNodes::BeginInputAttribute(h);
		ImGui::TextUnformatted("hue");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(s);
		ImGui::TextUnformatted("saturation");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(v);
		ImGui::TextUnformatted("value");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(a);
		ImGui::TextUnformatted("alpha");
		ImNodes::EndInputAttribute();

		ImGui::Spacing();

		ImNodes::BeginOutputAttribute(color);
		ImGui::TextUnformatted("color");
		ImNodes::EndOutputAttribute();
		return;
	case HSVDirection::COLOR_TO_HSVA:
		ImNodes::BeginInputAttribute(color);
		ImGui::TextUnformatted("color");
		ImNodes::EndInputAttribute();

		ImGui::Spacing();

		ImNodes::BeginOutputAttribute(h);
		ImGui::TextUnformatted("hue");
		ImNodes::EndOutputAttribute();
		ImNodes::BeginOutputAttribute(s);
		ImGui::TextUnformatted("saturation");
		ImNodes::EndOutputAttribute();
		ImNodes::BeginOutputAttribute(v);
		ImGui::TextUnformatted("value");
		ImNodes::EndOutputAttribute();
		ImNodes::BeginOutputAttribute(a);
		ImGui::TextUnformatted("alpha");
		ImNodes::EndOutputAttribute();
		return;
	}
}

NodeResult HSVNode::get_value(IdType id) const
{
	switch (direction) {
	case HSVDirection::HSVA_TO_COLOR: {
		const NodeResult h = get_parent()->value_from_attribute(this->h);
		const NodeResult s = get_parent()->value_from_attribute(this->s);
		const NodeResult v = get_parent()->value_from_attribute(this->v);
		const NodeResult a = get_parent()->value_from_attribute(this->a);

		if (std::ranges::any_of(std::initializer_list{ h, s, v, a }, [](const NodeResult& nr) { return nr.empty(); }))
			return {};

		float r = 0.0F;
		float g = 0.0F;
		float b = 0.0F;

		ImGui::ColorConvertHSVtoRGB(
			h.get<float>(),
			s.get<float>(),
			v.get<float>(),
			r,
			g,
			b);

		return ImColor(r, g, b, a.get<float>());
	}
	case HSVDirection::COLOR_TO_HSVA:
		const NodeResult color = get_parent()->value_from_attribute(this->color);

		if (color.empty())
			return {};

		const auto col = color.get<ImColor>();

		if (id == a)
			return col.Value.w;

		float h = 0.0F;
		float s = 0.0F;
		float v = 0.0F;

		ImGui::ColorConvertRGBtoHSV(
			col.Value.x,
			col.Value.y,
			col.Value.z,
			h,
			s,
			v);

		if (id == this->h)
			return h;
		if (id == this->s)
			return s;
		if (id == this->v)
			return v;
	}

	std::unreachable();
}

void HSVNode::serialize(nlohmann::json& output_json) const
{
	output_json["h"] = h;
	output_json["s"] = s;
	output_json["v"] = v;
	output_json["a"] = a;
	output_json["color"] = color;
}

void HSVNode::deserialize(const nlohmann::json& input_json)
{
	h = input_json["h"];
	s = input_json["s"];
	v = input_json["v"];
	a = input_json["a"];
	color = input_json["color"];
}
