#include "Features.hpp"

#include "Misc/Bhop.hpp"
#include "Visuals/ESP/ESP.hpp"
#include "Visuals/ForceCrosshair.hpp"
#include "Visuals/GrenadeHelper.hpp"
#include "Visuals/GrenadePrediction.hpp"
#include "Visuals/LoadingTextChanger.hpp"
#include "Visuals/Radar.hpp"

void Features::create()
{
	esp.emplace();
	force_crosshair.emplace();
	loading_text_changer.emplace();
	grenade_prediction.emplace();
	grenade_helper.emplace();
	bhop.emplace();
	radar.emplace();
}

void Features::destroy()
{
	radar.reset();
	bhop.reset();
	grenade_helper.reset();
	grenade_prediction.reset();
	loading_text_changer.reset();
	force_crosshair.reset();
	esp.reset();
}
