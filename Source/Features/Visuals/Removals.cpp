#include "Removals.hpp"

#include "../../SDK/Panorama/PanoramaUIEngine.hpp"
#include "../../SDK/Panorama/UIEngine.hpp"
#include "../../SDK/Panorama/UIPanel.hpp"

#include "../../Utils/Interval.hpp"

#include "../../Interfaces.hpp"

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

bool Removals::should_remove_sleeves() const
{
	return remove_sleeves.get();
}

void Removals::remove_ads_update() const
{
	if (!Interfaces::panorama_ui_engine)
		return;

	// Because we don't only check for equality on the pointer and never dereference it, it is fine for it to get outdated every now and then...
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
			engine->run_script(ads_panel, "$.GetContextPanel().SetHasClass('hidden', false);");
		}
		return;
	}

	UIEngine* engine = Interfaces::panorama_ui_engine->access_ui_engine();
	if (!engine)
		return;

	UIPanel* ads_panel = engine->find_panel("JsLeftColumn");
	engine->run_script(ads_panel, "$.GetContextPanel().SetHasClass('hidden', true);");
	was_removing_ads = true;
}
