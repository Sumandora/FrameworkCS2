#include "HSVNode.hpp"

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeCircuit.hpp"
#include "../NodeResult.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "nlohmann/json.hpp"

#include <optional>
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
		const std::optional<NodeResult> h = get_parent()->value_from_attribute(this->h);
		const std::optional<NodeResult> s = get_parent()->value_from_attribute(this->s);
		const std::optional<NodeResult> v = get_parent()->value_from_attribute(this->v);
		const std::optional<NodeResult> a = get_parent()->value_from_attribute(this->a);

		float r = 0.0F;
		float g = 0.0F;
		float b = 0.0F;

		ImGui::ColorConvertHSVtoRGB(
			h ? h->f : 0.0F,
			s ? s->f : 0.0F,
			v ? v->f : 0.0F,
			r,
			g,
			b);

		return { .color = ImColor(
					 r,
					 g,
					 b,
					 a ? a->f : 0.0F) };
	}
	case HSVDirection::COLOR_TO_HSVA:
		const std::optional<NodeResult> color = get_parent()->value_from_attribute(this->color);

		if (id == a)
			return { .f = color->color.Value.w };

		if (!color.has_value())
			return { .f = 0.0F };

		float h = 0.0F;
		float s = 0.0F;
		float v = 0.0F;

		ImGui::ColorConvertRGBtoHSV(
			color ? color->color.Value.x : 0.0F,
			color ? color->color.Value.y : 0.0F,
			color ? color->color.Value.z : 0.0F,
			h,
			s,
			v);

		if (id == this->h)
			return { .f = h };
		if (id == this->s)
			return { .f = s };
		if (id == this->v)
			return { .f = v };
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
