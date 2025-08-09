#include "../../GameHook.hpp"

#include "../../../../SDK/GameClass/GlowProperty.hpp"

#include "../../../../Features/Visuals/Glow.hpp"

#include "RetAddrSpoofer.hpp"

#include "imgui.h"

void Hooks::Game::GetGlowColor::hook_func(GlowProperty* glow_property, float* color)
{
	if (!glow->wants_modify(glow_property->entity())) {
		RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), glow_property, color);
		return;
	}

	const ImColor new_color = glow->get_color_for_entity(glow_property->entity());
	color[0] = new_color.Value.x;
	color[1] = new_color.Value.y;
	color[2] = new_color.Value.z;
	color[3] = new_color.Value.w;
}
