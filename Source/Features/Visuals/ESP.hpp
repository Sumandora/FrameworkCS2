#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"

#include "GenericESP/Element/Element.hpp"
#include "imgui.h"

#include "../../Utils/Logging.hpp"

#include "GenericESP/Element/Rectangle.hpp"

struct PlayerRectangle : Subgroup, GenericESP::Rectangle {
	Color color{ this, "Color" };
	FloatNumber rounding{ this, "Rounding", 0.0F, 10.0F };

	Subgroup rounded_edges{ this, "Rounded edges" };

	Checkbox rounding_top_left{ rounded_edges, "Top left" };
	Checkbox rounding_top_right{ rounded_edges, "Top right" };
	Checkbox rounding_bottom_left{ rounded_edges, "Bottom left" };
	Checkbox rounding_bottom_right{ rounded_edges, "Bottom right" };

	FloatNumber thickness{ this, "Thickness", 1.0F, 10.0F };
	Checkbox outlined{ this, "Outlined" };
	Color outline_color{ this, "Outline color" };
	FloatNumber outline_thickness{ this, "Outline thickness", 1.0F, 10.0F };
	Checkbox fill{ this, "Fill" };
	Color fill_color{ this, "Fill color" };

	using Subgroup::Subgroup;

	ImColor get_color(const GenericESP::EntityType* /*e*/) const override { return color.get(); }
	float get_rounding(const GenericESP::EntityType* /*e*/) const override { return rounding.get(); }

	bool get_rounding_top_left(const GenericESP::EntityType* /*e*/) const override { return rounding_top_left.get(); }
	bool get_rounding_top_right(const GenericESP::EntityType* /*e*/) const override { return rounding_top_right.get(); }
	bool get_rounding_bottom_left(const GenericESP::EntityType* /*e*/) const override { return rounding_bottom_left.get(); }
	bool get_rounding_bottom_right(const GenericESP::EntityType* /*e*/) const override { return rounding_bottom_right.get(); }

	float get_thickness(const GenericESP::EntityType* /*e*/) const override { return thickness.get(); }
	bool get_outlined(const GenericESP::EntityType* /*e*/) const override { return outlined.get(); }
	ImColor get_outline_color(const GenericESP::EntityType* /*e*/) const override { return outline_color.get(); }
	float get_outline_thickness(const GenericESP::EntityType* /*e*/) const override { return outline_thickness.get(); }
	bool get_fill(const GenericESP::EntityType* /*e*/) const override { return fill.get(); }
	ImColor get_fill_color(const GenericESP::EntityType* /*e*/) const override { return fill_color.get(); }
};

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled" };
	Subgroup lotto{ this, "Lotto" };
	Checkbox another_check{ lotto, "Another check" };
	Button btn{ lotto, "Click me", []() {
				   Logging::info("heya");
			   } };

	PlayerRectangle rect{ this, "Player ESP" };

public:
	ESP();

	void draw(ImDrawList* draw_list);
};
