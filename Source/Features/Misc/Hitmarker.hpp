#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Combo.hpp"
#include "../Settings/FloatSlider.hpp"
#include "../Settings/TextInput.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../GUI/Elements/EnumCombo.hpp"

#include "magic_enum/magic_enum.hpp"

#include <cstdint>
#include <ranges>
#include <string>
#include <utility>

struct GameEvent;

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
	Checkbox enabled{ this, "Enabled", false };
	Combo<HitMarkerSounds> sound{ this, "Sound" };
	TextInput custom_sound_name{ this, "Custom sound name", "sounds/buttons/bell1" };
	FloatSlider volume{ this, "Volume", 0.0F, 1.0F, 1.0F };

	std::string get_sound_path() const;

public:
	HitMarker();

	void event_handler(GameEvent* event);
};

inline UninitializedObject<HitMarker> hit_marker;
