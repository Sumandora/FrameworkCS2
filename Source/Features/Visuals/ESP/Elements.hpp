#pragma once

#include "GenericESP/Element/Bar.hpp"
#include "GenericESP/Element/Element.hpp"
#include "GenericESP/Element/Rectangle.hpp"
#include "GenericESP/Element/SidedElement.hpp"
#include "GenericESP/Element/SidedText.hpp"

#include "../../../GUI/GUI.hpp"
#include "../../../SDK/Entities/BaseEntity.hpp"

#include "../../Setting.hpp"
#include "../../Settings/Checkbox.hpp"
#include "../../Settings/Color.hpp"
#include "../../Settings/Combo.hpp"
#include "../../Settings/FloatSlider.hpp"
#include "../../Settings/MetaSetting.hpp"
#include "../../Settings/Subgroup.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <utility>

struct PlayerRectangle : MetaSetting, GenericESP::Rectangle {
	Checkbox enabled{ this, "Enabled", false };

	Checkbox health_based{ this, "Health-based color", false };
	Color color{ this, "Color" };
	Color alive_color{ this, "Alive color", ImColor{ 0.0F, 1.0F, 0.0F, 1.0F } };
	Color dead_color{ this, "Dead color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };

	FloatSlider rounding{ this, "Rounding", 0.0F, 10.0F, 0.0F };

	Subgroup rounded_edges{ this, "Rounded edges" };

	Checkbox rounding_top_left{ rounded_edges, "Top left", true };
	Checkbox rounding_top_right{ rounded_edges, "Top right", true };
	Checkbox rounding_bottom_left{ rounded_edges, "Bottom left", true };
	Checkbox rounding_bottom_right{ rounded_edges, "Bottom right", true };

	FloatSlider thickness{ this, "Thickness", 1.0F, 10.0F, 1.0F };
	Checkbox outlined{ this, "Outlined", true };
	Color outline_color{ this, "Outline color", ImColor{ 0.0F, 0.0F, 0.0F, 1.0F } };
	FloatSlider outline_thickness{ this, "Outline thickness", 1.0F, 10.0F, 1.0F };
	Checkbox fill{ this, "Fill", false };
	Color fill_color{ this, "Fill color" };

	using MetaSetting::MetaSetting;

	PlayerRectangle(SettingsHolder* parent, std::string name)
		: MetaSetting(parent, std::move(name))
	{
		color.add_visible_condition([this] { return !health_based.get(); });
		alive_color.add_visible_condition([this] { return health_based.get(); });
		dead_color.add_visible_condition([this] { return health_based.get(); });
	}

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

struct PlayerSidedText : MetaSetting, GenericESP::SidedText {
	Checkbox enabled{ this, "Enabled", false };

	FloatSlider spacing{ this, "Spacing", 0.0F, 10.0F, 3.0F };
	Combo<GenericESP::Side> side{ this, "Side", GenericESP::Side::TOP };
	FloatSlider font_scale{ this, "Font scale", 0.0F, 2.0F, 1.0F };
	Color font_color{ this, "Font color" };
	Checkbox shadow{ this, "Shadow", true };
	FloatSlider shadow_offset{ this, "Shadow offset", 1.0F, 5.0F, 1.0F };
	Color shadow_color{ this, "Shadow color", ImColor{ 0.0F, 0.0F, 0.0F, 1.0F } };

	using MetaSetting::MetaSetting;

	PlayerSidedText(SettingsHolder* parent, std::string name)
		: MetaSetting(parent, std::move(name))
	{
	}

	float get_spacing(const GenericESP::EntityType* /*e*/) const override { return spacing.get(); }
	GenericESP::Side get_side(const GenericESP::EntityType* /*e*/) const override { return side.get(); }
	ImFont* get_font(const GenericESP::EntityType* /*e*/) const override { return ImGui::GetFont(); }
	float get_font_size(const GenericESP::EntityType* /*e*/) const override { return GUI::get_base_font_size() * font_scale.get(); }
	ImColor get_font_color(const GenericESP::EntityType* /*e*/) const override { return font_color.get(); }
	bool get_shadow(const GenericESP::EntityType* /*e*/) const override { return shadow.get(); }
	float get_shadow_offset(const GenericESP::EntityType* /*e*/) const override { return shadow_offset.get(); }
	ImColor get_shadow_color(const GenericESP::EntityType* /*e*/) const override { return shadow_color.get(); }
};

struct PlayerBar : MetaSetting, GenericESP::BarWithText {
	Checkbox enabled{ this, "Enabled", false };

	Combo<GenericESP::Side> side{ this, "Side", GenericESP::Side::LEFT };

	Color background_color{ this, "Background color", ImColor{ 0.0F, 0.0F, 0.0F, 1.0F } };
	FloatSlider spacing{ this, "Spacing", 0.0F, 10.0F, 3.0F };
	FloatSlider width{ this, "Width", 0.0F, 10.0F, 3.0F };
	Color filled_color{ this, "Filled color", ImColor{ 0.0F, 1.0F, 0.0F, 1.0F } };
	Color empty_color{ this, "Empty color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };
	Checkbox gradient{ this, "Gradient", false };
	Checkbox flipped{ this, "Flipped", false };

	Checkbox outlined{ this, "Outlined", true };

	Color outline_color{ this, "Outline color", ImColor{ 0.0F, 0.0F, 0.0F, 1.0F } };
	FloatSlider outline_thickness{ this, "Outline thickness", 0.0F, 10.0F, 1.0F };

	Checkbox text_enabled{ this, "Text enabled", false };

	Subgroup text_group{ this, "Text group" };
	Checkbox hide_when_full{ text_group, "Hide when full", true };

	FloatSlider font_scale{ text_group, "Font scale", 0.0F, 2.0F, 1.0F };
	Color font_color{ text_group, "Font color" };
	Checkbox shadow{ text_group, "Shadow", true };
	FloatSlider shadow_offset{ text_group, "Shadow offset", 1.0F, 5.0F, 1.0F };
	Color shadow_color{ text_group, "Shadow color", ImColor{ 0.0F, 0.0F, 0.0F, 1.0F } };

	PlayerBar(SettingsHolder* parent, std::string name, PercentageProvider percentage_provider, Provider text_provider)
		: MetaSetting(parent, std::move(name))
		, GenericESP::BarWithText(std::move(percentage_provider), std::move(text_provider))
	{
	}

	GenericESP::Side get_side(const GenericESP::EntityType* /*e*/) const override { return side.get(); }
	ImColor get_background_color(const GenericESP::EntityType* /*e*/) const override { return background_color.get(); }
	float get_spacing(const GenericESP::EntityType* /*e*/) const override { return spacing.get(); }
	float get_width(const GenericESP::EntityType* /*e*/) const override { return width.get(); }
	ImColor get_filled_color(const GenericESP::EntityType* /*e*/) const override { return filled_color.get(); }
	ImColor get_empty_color(const GenericESP::EntityType* /*e*/) const override { return empty_color.get(); }
	bool get_gradient(const GenericESP::EntityType* /*e*/) const override { return gradient.get(); }
	int get_hue_steps(const GenericESP::EntityType* /*e*/) const override { return 5; /* Standards :) */ }
	bool get_flipped(const GenericESP::EntityType* /*e*/) const override { return flipped.get(); }
	bool get_outlined(const GenericESP::EntityType* /*e*/) const override { return outlined.get(); }
	ImColor get_outline_color(const GenericESP::EntityType* /*e*/) const override { return outline_color.get(); }
	float get_outline_thickness(const GenericESP::EntityType* /*e*/) const override { return outline_thickness.get(); }

	bool get_text_enabled(const GenericESP::EntityType* /*e*/) const override { return text_enabled.get(); }
	bool get_hide_when_full(const GenericESP::EntityType* /*e*/) const override { return hide_when_full.get(); }

	ImFont* get_font(const GenericESP::EntityType* /*e*/) const override { return ImGui::GetFont(); }
	float get_font_size(const GenericESP::EntityType* /*e*/) const override { return ImGui::GetFontSize() * font_scale.get(); }
	ImColor get_font_color(const GenericESP::EntityType* /*e*/) const override { return font_color.get(); }
	bool get_shadow(const GenericESP::EntityType* /*e*/) const override { return shadow.get(); }
	float get_shadow_offset(const GenericESP::EntityType* /*e*/) const override { return shadow_offset.get(); }
	ImColor get_shadow_color(const GenericESP::EntityType* /*e*/) const override { return shadow_color.get(); }
};
