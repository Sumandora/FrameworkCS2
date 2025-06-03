#include "../Tabs.hpp"

#include "imgui.h"

#include "../../../Interfaces.hpp"

#include "../../../SDK/Panorama/PanoramaUIEngine.hpp"
#include "../../../SDK/Panorama/UIEngine.hpp"
#include "../../../SDK/Panorama/UIPanel.hpp"

#include "../../../Hooks/Game/GameHook.hpp"

#include "misc/cpp/imgui_stdlib.h"

#include <mutex>
#include <string>

/** TODO Make this a feature:
var obj = $.GetContextPanel().FindChildTraverse("JsLeftColumn");
obj.visible = false;
*/

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
			static std::string panel = "CSGOMainMenu";
			ImGui::InputText("Panel", &panel);
			static std::string context = "panorama/layout/base.xml";
			ImGui::InputText("Context", &context);
			static std::string source;
			ImGui::InputTextMultiline("Source", &source);

			if (ImGui::Button("Execute!")) {
				UIEngine* ui_engine = Interfaces::panorama_ui_engine->access_ui_engine();
				UIPanel* ui_panel = ui_engine->find_panel(panel);
				Hooks::Game::FrameStageNotify::thread_executor.queue([ui_engine,
																		 ui_panel,
																		 s = std::string{ source },
																		 c = std::string{ context }] {
					ui_engine->run_script(ui_panel, s.c_str(), c.c_str());
				});
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}
