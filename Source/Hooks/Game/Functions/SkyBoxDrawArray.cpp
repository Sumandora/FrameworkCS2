#include "../GameHook.hpp"

#include "../../../Features/Visuals/WorldColors.hpp"

#include "RetAddrSpoofer.hpp"

void __attribute((optimize("O0"))) Hooks::Game::SkyBoxDrawArray::hook_func(void* sky_box_object_desc, void* render_context, MeshDrawPrimitive* mesh_array,
	int mesh_count, void* scene_view, void* scene_layer, void* scene_system_per_frame_stats)
{
	const bool hide_original
		= world_colors->handle_sky(mesh_array, mesh_count,
			[&](MeshDrawPrimitive* mesh_array, int mesh_count) {
				RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()),
					sky_box_object_desc, render_context, mesh_array,
					mesh_count, scene_view, scene_layer, scene_system_per_frame_stats);
			});

	if (hide_original)
		return;

	RetAddrSpoofer::invoke<void>(
		reinterpret_cast<void*>(hook->get_trampoline()),
		sky_box_object_desc, render_context, mesh_array,
		mesh_count, scene_view, scene_layer, scene_system_per_frame_stats);
}
