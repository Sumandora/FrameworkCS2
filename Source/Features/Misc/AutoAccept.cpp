#include "AutoAccept.hpp"

#include "../Feature.hpp"

#include "../../SDK/Panorama/PanoramaUIEngine.hpp"
#include "../../SDK/Panorama/UIEngine.hpp"
#include "../../SDK/Panorama/UIPanel.hpp"

#include "../../Interfaces.hpp"

#include "../../Utils/Logging.hpp"

#include <chrono>

AutoAccept::AutoAccept()
	: Feature("Misc", "Auto accept")
{
}

void AutoAccept::on_match_found()
{
	if (!enabled.get())
		return;

	match_found_time = std::chrono::system_clock::now();
	awaits_accept = true;

	Logging::debug("Found match!");
}

void AutoAccept::update()
{
	if (!awaits_accept)
		return;

	if (std::chrono::system_clock::now() - match_found_time <= std::chrono::milliseconds{ static_cast<int>(delay.get() * 1000.0F) })
		return;

	UIEngine* ui_engine = Interfaces::panorama_ui_engine->access_ui_engine();
	if (!ui_engine)
		return;

	UIPanel* ui_panel = ui_engine->find_panel("CSGOMainMenu");
	if (!ui_panel)
		return;

	ui_engine->run_script(ui_panel, "$.DispatchEvent(\"MatchAssistedAccept\");", "panorama/layout/base.xml");
	awaits_accept = false;

	Logging::debug("Match accepted!");
}
