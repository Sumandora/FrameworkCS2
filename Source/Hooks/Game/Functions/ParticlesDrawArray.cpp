#include "../GameHook.hpp"

#include "../../../SDK/GameClass/MeshDrawPrimitive.hpp"

#include "../../../Features/Visuals/WorldColors.hpp"

#include "RetAddrSpoofer.hpp"

void __attribute((optimize("O0"))) Hooks::Game::ParticlesDrawArray::hook_func(void* particle_object_desc, void* render_context, MeshDrawPrimitive* mesh_array,
	int mesh_count, void* scene_view, void* scene_layer, void* scene_system_per_frame_stats)
{
	world_colors->handle_particle(mesh_array, mesh_count);

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()),
		particle_object_desc, render_context, mesh_array,
		mesh_count, scene_view, scene_layer, scene_system_per_frame_stats);
}
