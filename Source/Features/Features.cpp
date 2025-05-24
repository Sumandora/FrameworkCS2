#include "Features.hpp"

void Features::create()
{
	esp.emplace();
}

void Features::destroy()
{
	esp.reset();
}
