#include "Features.hpp"

#include "Combat/Aimbot.hpp"
#include "Misc/AutoStrafer.hpp"
#include "Misc/Bhop.hpp"
#include "Misc/Hitmarker.hpp"
#include "Visuals/Chams/Chams.hpp"
#include "Visuals/ESP/ESP.hpp"
#include "Visuals/ForceCrosshair.hpp"
#include "Visuals/GrenadeHelper.hpp"
#include "Visuals/GrenadePrediction.hpp"
#include "Visuals/LoadingTextChanger.hpp"
#include "Visuals/ModelChanger/ModelChanger.hpp"
#include "Visuals/Radar.hpp"
#include "Visuals/Removals.hpp"

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
}

void Features::destroy()
{
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
