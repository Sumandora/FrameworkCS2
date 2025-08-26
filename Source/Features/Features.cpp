#include "Features.hpp"

#include "Combat/Aimbot.hpp"
#include "Griefing/AutoPickup.hpp"
#include "Griefing/BlockBot.hpp"
#include "Misc/AutoAccept.hpp"
#include "Misc/AutoStrafer.hpp"
#include "Misc/Bhop.hpp"
#include "Misc/BombTimer.hpp"
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
#include "Visuals/ModelChanger/ModelChanger.hpp"
#include "Visuals/Radar.hpp"
#include "Visuals/Removals.hpp"
#include "Visuals/WorldColors.hpp"
#include <tuple>

static constexpr std::tuple ALL_FEATURES{
	&esp,
	&force_crosshair,
	&grenade_prediction,
	&grenade_helper,
	&bhop,
	&radar,
	&aimbot,
	&auto_strafer,
	&removals,
	&chams,
	&model_changer,
	&hit_marker,
	&force_third_person,
	&bomb_timer,
	&vote_revealer,
	&world_colors,
	&player_list,
	&bullet_tracers,
	&field_of_view,
	&glow,
	&auto_pickup,
	&block_bot,
	&auto_accept,
};

void Features::create()
{
	std::apply([](auto&... feature) { (feature->emplace(), ...); }, ALL_FEATURES);

	initialized = true;
}

void Features::destroy()
{
	std::apply([](auto&... feature) { (..., feature->reset()); }, ALL_FEATURES);
}
