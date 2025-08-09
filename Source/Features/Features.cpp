#include "Features.hpp"

#include "Combat/Aimbot.hpp"
#include "Misc/AutoStrafer.hpp"
#include "Misc/Bhop.hpp"
#include "Misc/BombTimer.hpp"
#include "Misc/Griefing.hpp"
#include "Misc/Hitmarker.hpp"
#include "Misc/VoteRevealer/VoteRevealer.hpp"
#include "PlayerList/PlayerList.hpp"
#include "Visuals/BulletTracers.hpp"
#include "Visuals/Chams/Chams.hpp"
#include "Visuals/ESP/ESP.hpp"
#include "Visuals/FieldOfView.hpp"
#include "Visuals/ForceCrosshair.hpp"
#include "Visuals/ForceThirdPerson.hpp"
#include "Visuals/Glow.hpp"
#include "Visuals/GrenadeHelper.hpp"
#include "Visuals/GrenadePrediction.hpp"
#include "Visuals/LoadingTextChanger.hpp"
#include "Visuals/ModelChanger/ModelChanger.hpp"
#include "Visuals/Radar.hpp"
#include "Visuals/Removals.hpp"
#include "Visuals/WorldColors.hpp"

void Features::create()
{
	esp.emplace();
	force_crosshair.emplace();
	loading_text_changer.emplace();
	grenade_prediction.emplace();
	grenade_helper.emplace();
	bhop.emplace();
	radar.emplace();
	aimbot.emplace();
	auto_strafer.emplace();
	removals.emplace();
	chams.emplace();
	model_changer.emplace();
	hit_marker.emplace();
	force_third_person.emplace();
	bomb_timer.emplace();
	vote_revealer.emplace();
	world_colors.emplace();
	player_list.emplace();
	bullet_tracers.emplace();
	field_of_view.emplace();
	glow.emplace();
	griefing.emplace();

	initialized = true;
}

void Features::destroy()
{
	griefing.reset();
	glow.reset();
	field_of_view.reset();
	bullet_tracers.reset();
	player_list.reset();
	world_colors.reset();
	vote_revealer.reset();
	bomb_timer.reset();
	force_third_person.reset();
	hit_marker.reset();
	model_changer.reset();
	chams.reset();
	removals.reset();
	auto_strafer.reset();
	aimbot.reset();
	radar.reset();
	bhop.reset();
	grenade_helper.reset();
	grenade_prediction.reset();
	loading_text_changer.reset();
	force_crosshair.reset();
	esp.reset();
}
