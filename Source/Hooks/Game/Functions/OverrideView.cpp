#include "../GameHook.hpp"

#include "../../../Features/Visuals/ForceThirdPerson.hpp"
#include "../../../Features/Visuals/Removals.hpp"

#include "RetAddrSpoofer.hpp"

void Hooks::Game::OverrideView::hook_func(ViewRender* thisptr, ViewSetup* view_setup)
{
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, view_setup);

	force_third_person->override_view(view_setup);
	removals->override_view(view_setup);
}
