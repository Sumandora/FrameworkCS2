#include "Removals.hpp"

#include "../../SDK/GameClass/CSGOInput.hpp"
#include "../../SDK/GameClass/ViewSetup.hpp"
#include "../../SDK/Panorama/PanoramaUIEngine.hpp"
#include "../../SDK/Panorama/UIEngine.hpp"
#include "../../SDK/Panorama/UIPanel.hpp"

#include "../../Utils/Interval.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include "../Feature.hpp"

#include <chrono>

Removals::Removals()
	: Feature("Visuals", "Removals")
{
}

bool Removals::should_remove_legs() const
{
	return remove_legs.get();
}

bool Removals::should_remove_scope() const
{
	return remove_scope.get();
}

bool Removals::should_remove_flash_overlay() const
{
	return remove_flash_overlay.get();
}

void Removals::override_view(ViewSetup* view_setup)
{
	if (!remove_punch.get())
		return;

	// TODO at some point I would like to separate those but I feel like those would amount to 3 hooks...
	// This implicitly removes any kind of view offsets, like view punch, aim punch, screen shake and so on...
	view_setup->view_angles() = Memory::csgo_input->view_angles();
}

void Removals::remove_ads_update() const
{
	if (!Interfaces::panorama_ui_engine)
		return;

	static Interval<std::chrono::seconds, 1> update_interval;
	if (!update_interval.passed())
		return;

	static bool was_removing_ads = false;
	if (!remove_ads.get()) {
		if (was_removing_ads) {
			was_removing_ads = false;

			UIEngine* engine = Interfaces::panorama_ui_engine->access_ui_engine();
			if (!engine)
				return;

			UIPanel* ads_panel = engine->find_panel("JsLeftColumn");
			if (ads_panel) {
				engine->run_script(ads_panel, "$.GetContextPanel().SetHasClass('hidden', false);");
			}
		}
		return;
	}

	UIEngine* engine = Interfaces::panorama_ui_engine->access_ui_engine();
	if (!engine)
		return;

	UIPanel* ads_panel = engine->find_panel("JsLeftColumn");
	if (ads_panel) {
		engine->run_script(ads_panel, "$.GetContextPanel().SetHasClass('hidden', true);");
		was_removing_ads = true;
	}
}
