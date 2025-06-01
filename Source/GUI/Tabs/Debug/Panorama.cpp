#include "../Tabs.hpp"

#include "imgui.h"

#include "../../../Interfaces.hpp"

#include "../../../SDK/Panorama/PanoramaUIEngine.hpp"
#include "../../../SDK/Panorama/UIEngine.hpp"
#include "../../../SDK/Panorama/UIPanel.hpp"

#include "../../../Utils/Logging.hpp"

#include "../../../Hooks/Game/GameHook.hpp"

#include <mutex>
#include <string>

void GUI::Tabs::Debug::draw_panorama()
{
	if (ImGui::BeginTabBar("#Panorama debug tabs", ImGuiTabBarFlags_Reorderable)) {
		if (ImGui::BeginTabItem("Panels")) {
			Interfaces::panorama_ui_engine->access_ui_engine()->for_each_panel([&](UIPanel* panel) {
				ImGui::Text("%p -> %s", static_cast<void*>(panel), panel->id);
			});
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Execute Javascript")) {
			static char panel[512] = "CSGOMainMenu";
			ImGui::InputText("Panel", panel, IM_ARRAYSIZE(panel));
			static char context[512] = "panorama/layout/base.xml";
			ImGui::InputText("Context", context, IM_ARRAYSIZE(context));
			static char source[4096] = "";
			ImGui::InputTextMultiline("Source", source, IM_ARRAYSIZE(source));

			if (ImGui::Button("Execute!")) {
				UIEngine* ui_engine = Interfaces::panorama_ui_engine->access_ui_engine();
				Logging::info("Engine: {}", ui_engine);
				UIPanel* ui_panel = ui_engine->find_panel(panel);
				Logging::info("Panel: {}", ui_panel);
				const std::lock_guard g{ Hooks::Game::FrameStageNotify::queued_tasks_lock };
				Hooks::Game::FrameStageNotify::queued_tasks.emplace_back(
					[ui_engine,
						ui_panel,
						s = std::string{ source },
						c = std::string{ context }]() {
						ui_engine->run_script(ui_panel, s.c_str(), c.c_str());
					});
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
