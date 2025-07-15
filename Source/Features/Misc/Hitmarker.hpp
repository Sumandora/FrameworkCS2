#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"
#include "../Settings/Combo.hpp"
#include "../Settings/FloatSlider.hpp"
#include "../Settings/Section.hpp"
#include "../Settings/TextInput.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../GUI/Elements/EnumCombo.hpp"

#include "magic_enum/magic_enum.hpp"

#include <chrono>
#include <cstdint>
#include <ranges>
#include <string>
#include <utility>

struct GameEvent;

struct ImDrawList;

enum class HitMarkerSounds : std::uint8_t {
	// NOLINTBEGIN(readability-identifier-naming)
	Bell,
	Latch_unlocked,
	Light_power_on_switch,
	Custom,
	// NOLINTEND(readability-identifier-naming)
};

template <>
struct EnumNames<HitMarkerSounds> {
	static constexpr auto NAMES = magic_enum::enum_entries<HitMarkerSounds>()
		| std::ranges::views::transform([](const auto& pair) {
			  std::string no_underscores{ pair.second };
			  for (char& c : no_underscores) {
				  if (c == '_')
					  c = ' ';
			  }
			  return std::pair<HitMarkerSounds, std::string>{ pair.first, no_underscores };
		  });
};

class HitMarker : public Feature {
	Section hit_marker{ this, "Hit marker" };
	Checkbox hit_marker_enabled{ hit_marker, "Enabled", false };
	FloatSlider length{ hit_marker, "Length", 0.0F, 10.0F, 5.0F };
	FloatSlider gap{ hit_marker, "Gap", 0.0F, 10.0F, 5.0F };
	FloatSlider timeout{ hit_marker, "Timeout", 0.0F, 2000.0F, 500.0F }; // TODO Int slider or preferably a time/time-duration setting
	Color color{ hit_marker, "Color" };

	Section hit_sound{ this, "Hit sound" };
	Checkbox hit_sound_enabled{ hit_sound, "Enabled", false };
	Combo<HitMarkerSounds> sound{ hit_sound, "Sound" };
	TextInput custom_sound_name{ hit_sound, "Custom sound name", "sounds/buttons/bell1" };
	FloatSlider volume{ hit_sound, "Volume", 0.0F, 1.0F, 1.0F };

	std::string get_sound_path() const;
	std::chrono::system_clock::time_point last_hurt;

public:
	HitMarker();

	void event_handler(GameEvent* event);
	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<HitMarker> hit_marker;
