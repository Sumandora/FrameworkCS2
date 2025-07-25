#include "Hitmarker.hpp"

#include "../Feature.hpp"

#include "../../SDK/ChatPrintf.hpp"
#include "../../SDK/ConVar/ConVar.hpp"
#include "../../SDK/ConVar/EngineCvar.hpp"
#include "../../SDK/Entities/CSPlayerController.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/EngineToClient.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"

#include "../../Notifications/Notifications.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "../../Utils/HTMLEscape.hpp"
#include "../../Utils/Logging.hpp"

#include "magic_enum/magic_enum.hpp"

#include "imgui.h"

#include <array>
#include <chrono>
#include <cstdlib>
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

	player_color.add_visible_condition([this] { return output.get_raw() == HitLogsOutput::Chat; });
	damage_color.add_visible_condition([this] { return output.get_raw() == HitLogsOutput::Chat; });
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

void HitMarker::do_hit_sound() const
{
	// TODO: This is hacky, I would prefer to just call game functions
	static constexpr std::format_string<const float&, const std::string&> COMMAND
		= "snd_toolvolume {};"
		  "play {}";

	const float volume = this->volume.get() * ::volume->get_float(); // Since tool sounds don't seem to be scaled with the global volume, just do it ourselves.
	const std::string sound_path = get_sound_path();

	const std::string command = std::format(COMMAND, volume, sound_path);

	Interfaces::engine->execute_client_cmd(command.c_str());
}

void HitMarker::do_hit_logs(CSPlayerPawn* victim, int damage) const
{
	CSPlayerController* controller = victim->original_controller().get();
	if (!controller)
		return;

	switch (output.get()) {
	case HitLogsOutput::Notifications: {
		const std::string message = std::format("Hurt {} for {} hp", controller->get_decorated_player_name(), damage);
		Notifications::create("Player hurt", message, Notifications::Severity::INFO);
		break;
	}
	case HitLogsOutput::Chat: {
		std::string victim_name = controller->get_decorated_player_name();

		// This is printed into the panorama view, so it needs to be escaped
		html_escape(victim_name);

		ChatPrintf::print(R"(Hurt <font color="{}">{}</font> for <font color="{}">{}</font> hp)", ChatPrintf::to_html_rgba(player_color.get()), victim_name, ChatPrintf::to_html_rgba(damage_color.get()), damage);
		break;
	}
	case HitLogsOutput::Standard_Output:
		Logging::info("Hurt {} for {} hp", controller->get_decorated_player_name(), damage);
		break;
	}
}

void HitMarker::event_handler(GameEvent* game_event)
{
	const bool hit_sound_enabled = this->hit_sound_enabled.get();
	const bool hit_logs_enabled = this->hit_logs_enabled.get();
	if (!hit_marker_enabled.get() && !hit_sound_enabled && !hit_logs_enabled)
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

	if (victim == Memory::local_player)
		return; // Should I leave those be? Setting perhaps (TODO)

	auto* cs_player_pawn = victim->entity_cast<CSPlayerPawn*>();
	if (!cs_player_pawn)
		return;

	last_hurt = std::chrono::system_clock::now();

	if (hit_sound_enabled)
		do_hit_sound();

	if (hit_logs_enabled)
		do_hit_logs(cs_player_pawn, game_event->get_int("dmg_health", 0));
}

void HitMarker::draw(ImDrawList* draw_list)
{
	if (!hit_marker_enabled.get())
		return;

	const auto now = std::chrono::system_clock::now();
	const float timeout_ms = this->timeout.get();
	const auto timeout = std::chrono::milliseconds{ static_cast<int>(timeout_ms) };

	if (now - last_hurt >= timeout)
		return;

	const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_hurt);
	const float progress = static_cast<float>(elapsed.count()) / timeout_ms;

	ImColor color = this->color.get();
	color.Value.w *= 1.0F - progress;

	const float gap = this->gap.get();
	const ImVec2 gap_vec{ gap, gap };

	const float length = this->length.get();
	const ImVec2 length_vec{ length, length };

	const ImVec2 center = ImGui::GetIO().DisplaySize / 2.0F;

	// TODO use genericesp line for shadows and more settings in general...
	draw_list->AddLine(center - gap_vec, center - gap_vec - length_vec, color);
	draw_list->AddLine(center + gap_vec, center + gap_vec + length_vec, color);

	const ImVec2 gap_flip_vec{ gap, -gap };
	const ImVec2 length_flip_vec{ length, -length };

	draw_list->AddLine(center - gap_flip_vec, center - gap_flip_vec - length_flip_vec, color);
	draw_list->AddLine(center + gap_flip_vec, center + gap_flip_vec + length_flip_vec, color);
}
