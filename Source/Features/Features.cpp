#include "Features.hpp"

#include "Visuals/ESP/ESP.hpp"
#include "Visuals/ForceCrosshair.hpp"
#include "Visuals/LoadingTextChanger.hpp"
#include "Visuals/GrenadePrediction.hpp"

void Features::create()
{
	esp.emplace();
	force_crosshair.emplace();
	loading_text_changer.emplace();
	grenade_prediction.emplace();
}

void Features::destroy()
{
	grenade_prediction.reset();
	loading_text_changer.reset();
	force_crosshair.reset();
	esp.reset();
}
