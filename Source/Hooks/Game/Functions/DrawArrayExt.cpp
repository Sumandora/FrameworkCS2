#include "../GameHook.hpp"

#include "../../../Features/Visuals/Chams/Chams.hpp"

#include "RetAddrSpoofer.hpp"

void __attribute((optimize("O0"))) Hooks::Game::DrawArrayExt::hook_func(void* animatable_scene_object_desc, void* render_context, MeshDrawPrimitive* mesh_array,
	int mesh_count, void* scene_view, void* scene_layer, void* scene_system_per_frame_stats, void* material)
{
	bool hide_original = false;

	hide_original = hide_original
		|| chams->draw_object(mesh_array, mesh_count, [&](MeshDrawPrimitive* mesh_array, int mesh_count) {
			   RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()),
				   animatable_scene_object_desc, render_context, mesh_array,
				   mesh_count, scene_view, scene_layer, scene_system_per_frame_stats, material);
		   });

	if (hide_original)
		return;

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()),
		animatable_scene_object_desc, render_context, mesh_array,
		mesh_count, scene_view, scene_layer, scene_system_per_frame_stats, material);
}
