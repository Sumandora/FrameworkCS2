#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/FieldOfView.hpp"

#include "glm/ext/vector_float3.hpp"

void Hooks::Game::CalculateViewModelPosition::hook_func(void* thisptr, glm::vec3* position, float* fov)
{
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, position, fov);

	if(field_of_view->modifies_viewmodel()) {
		field_of_view->modify_viewmodel_fov(fov);
		field_of_view->modify_viewmodel_position(position);
	}
}
