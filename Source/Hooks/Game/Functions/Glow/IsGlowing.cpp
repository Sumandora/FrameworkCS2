#include "../../GameHook.hpp"

#include "../../../../SDK/GameClass/GlowProperty.hpp"

#include "../../../../Features/Visuals/Glow.hpp"

#include "RetAddrSpoofer.hpp"

bool Hooks::Game::IsGlowing::hook_func(GlowProperty* glow_property)
{
	if (!glow->wants_modify(glow_property->entity())) {
		return RetAddrSpoofer::invoke<bool>(reinterpret_cast<void*>(hook->get_trampoline()), glow_property);
	}

	return true;
}
