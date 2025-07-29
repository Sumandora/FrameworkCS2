#include "Radar.hpp"

#include "../Feature.hpp"

#include "../../SDK/Padding.hpp"

#include <cstdint>

Radar::Radar()
	: Feature("Visuals", "Radar")
{
}

struct GameRadar {
	PADDING(0x17800); // Take a look in the function that is hooked, the RadarUpdate one, you should find that offset by looking at the bitors.
	std::uint8_t unk_bool;
};

void Radar::on_radar_update(GameRadar* radar)
{
	if (!enabled.get())
		return;

	// TODO figure out how to lay out the bit fields for this calculation to happen naturally
	radar->unk_bool = (radar->unk_bool & 0xfe) | 1;
}
