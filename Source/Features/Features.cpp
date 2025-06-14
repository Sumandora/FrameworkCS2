#include "Features.hpp"

#include "Visuals/ESP/ESP.hpp"
#include "Visuals/ForceCrosshair.hpp"
#include "Visuals/LoadingTextChanger.hpp"

void Features::create()
{
	esp.emplace();
	force_crosshair.emplace();
	loading_text_changer.emplace();
}

void Features::destroy()
{
	loading_text_changer.reset();
	force_crosshair.reset();
	esp.reset();
}
