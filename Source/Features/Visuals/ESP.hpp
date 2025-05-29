#pragma once

#include "../Feature.hpp"
#include "../Setting.hpp"

#include "GenericESP/Element/Element.hpp"
#include "GenericESP/Element/Rectangle.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "../../Utils/Logging.hpp"

#include "../../SDK/Entities/BaseEntity.hpp"

struct PlayerRectangle : MetaSetting, GenericESP::Rectangle {
	Checkbox enabled{ this, "Enabled", false };

	Checkbox health_based{ this, "Health-based color", false };
	Color color = Color{ this, "Color" }
					  .visible_condition([this] { return !health_based.get(); });
	Color alive_color = Color{ this, "Alive color", { 0.0F, 1.0F, 0.0F, 1.0F } }
							.visible_condition([this] { return health_based.get(); });
	Color dead_color = Color{ this, "Dead color", { 1.0F, 0.0F, 0.0F, 1.0F } }
						   .visible_condition([this] { return health_based.get(); });

	FloatSlider rounding{ this, "Rounding", 0.0F, 10.0F };

	Subgroup rounded_edges{ this, "Rounded edges" };

	Checkbox rounding_top_left{ rounded_edges, "Top left", true };
	Checkbox rounding_top_right{ rounded_edges, "Top right", true };
	Checkbox rounding_bottom_left{ rounded_edges, "Bottom left", true };
	Checkbox rounding_bottom_right{ rounded_edges, "Bottom right", true };

	FloatSlider thickness{ this, "Thickness", 1.0F, 10.0F };
	Checkbox outlined{ this, "Outlined", true };
	Color outline_color{ this, "Outline color" };
	FloatSlider outline_thickness{ this, "Outline thickness", 1.0F, 10.0F };
	Checkbox fill{ this, "Fill", false };
	Color fill_color{ this, "Fill color" };

	using MetaSetting::MetaSetting;

	ImColor get_color(const GenericESP::EntityType* e) const override
	{
		if (health_based.get()) {
			const auto* entity = static_cast<const BaseEntity*>(e);
			const ImColor a = dead_color.get();
			const ImColor b = alive_color.get();

			const auto health = static_cast<float>(entity->health());
			const auto max = static_cast<float>(entity->max_health());
			
			return ImLerp(a.Value, b.Value, health / max);
		}
		return color.get();
	}
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
	Checkbox enabled{ this, "Enabled", false };
	Subgroup lotto{ this, "Lotto" };
	Checkbox another_check{ lotto, "Another check", true };
	Button btn{ lotto, "Click me", []() {
				   Logging::info("heya");
			   } };

	Tabs elements{ this, "Elements" };
	PlayerRectangle box{ elements, "Box" };

public:
	ESP();

	void draw(ImDrawList* draw_list);
};
