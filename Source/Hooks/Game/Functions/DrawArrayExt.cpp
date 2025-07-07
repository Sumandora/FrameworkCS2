#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../SDK/GameClass/MeshDrawPrimitive.hpp"

#include "imgui.h"

void Hooks::Game::DrawArrayExt::hook_func(void* animatable_scene_object_desc, void* render_context, MeshDrawPrimitive* mesh_array,
	int mesh_count, void* scene_view, void* scene_layer, void* scene_system_per_frame_stats, void* material)
{
	for (int i = 0; i < mesh_count; i++) {
		MeshDrawPrimitive& mesh_draw_primitive = mesh_array[i];
		mesh_draw_primitive.color = ImColor{ 255, 0, 0, 255 };
	}

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()),
		animatable_scene_object_desc, render_context, mesh_array,
		mesh_count, scene_view, scene_layer, scene_system_per_frame_stats, material);
}
