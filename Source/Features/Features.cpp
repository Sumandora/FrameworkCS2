#include "Features.hpp"

void Features::create()
{
	esp.emplace();
	force_crosshair.emplace();
}

void Features::destroy()
{
	force_crosshair.reset();
	esp.reset();
}
