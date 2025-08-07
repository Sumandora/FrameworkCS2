#include "../GameHook.hpp"

#include "../../../Features/Visuals/FieldOfView.hpp"

#include "RetAddrSpoofer.hpp"

#include <optional>

float Hooks::Game::GetFov::hook_func(void* thisptr)
{
	if (std::optional<float> optional_fov = field_of_view->get_fov();
		optional_fov.has_value()) {
		return optional_fov.value();
	}

	return RetAddrSpoofer::invoke<float>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr);
}
