#include "Hitmarker.hpp"

#include "../Feature.hpp"

#include "../../SDK/ConVar/ConVar.hpp"
#include "../../SDK/ConVar/EngineCvar.hpp"
#include "../../SDK/Entities/CSPlayerController.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/EngineToClient.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "../../Utils/Logging.hpp"

#include "magic_enum/magic_enum.hpp"

#include <array>
#include <cstring>
#include <format>
#include <string>
#include <utility>

static ConVar* volume = nullptr;

HitMarker::HitMarker()
	: Feature("Misc", "Hit marker")
{
	custom_sound_name.add_visible_condition([this] { return sound.get_raw() == HitMarkerSounds::Custom; });

	::volume = Interfaces::engineCvar->findByName("volume");
}

static constexpr std::array HIT_MARKER_SOUNDS{
	"sounds/buttons/bell1",
	"sounds/buttons/latchunlocked2",
	"sounds/buttons/light_power_on_switch_01",
};

static_assert(HIT_MARKER_SOUNDS.size() == magic_enum::enum_count<HitMarkerSounds>() - 1 /* Custom value doesn't count */);

std::string HitMarker::get_sound_path() const
{
	const HitMarkerSounds sound = this->sound.get();
	if (sound == HitMarkerSounds::Custom)
		return custom_sound_name.get();
	return HIT_MARKER_SOUNDS[std::to_underlying(sound)];
}

void HitMarker::event_handler(GameEvent* game_event)
{
	if (!enabled.get())
		return;

	if (std::strcmp(game_event->GetName(), "player_hurt") != 0)
		return;

	BaseEntity* attacker = game_event->get_entity("attacker");
	if (!attacker)
		return;

	if (attacker != Memory::local_player)
		return;

	BaseEntity* victim = game_event->get_entity("userid");
	if (!victim)
		return;

	if (!victim->entity_cast<CSPlayerPawn*>())
		return;

	// TODO: This is hacky, I would prefer to just call game functions
	static constexpr std::format_string<const float&, const std::string&> COMMAND
		= "snd_toolvolume {};"
		  "play {}";

	const float volume = this->volume.get() * ::volume->get_float(); // Since tool sounds don't seem to be scaled with the global volume, just do it ourselves.
	const std::string sound_path = get_sound_path();

	const std::string command = std::format(COMMAND, volume, sound_path);

	Interfaces::engine->execute_client_cmd(command.c_str());
}
