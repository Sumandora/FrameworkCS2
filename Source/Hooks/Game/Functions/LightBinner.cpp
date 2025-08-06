#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/WorldColors.hpp"

void Hooks::Game::LightBinner::hook_func(void* thisptr, SceneLightObject* scene_light_object, void* rdx)
{
	world_colors->handle_light(scene_light_object);

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, scene_light_object, rdx);
}
