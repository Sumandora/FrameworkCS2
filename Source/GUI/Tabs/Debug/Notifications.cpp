#include "../Tabs.hpp"

#include "../../../Notifications/Notifications.hpp"

#include "imgui.h"
#include "magic_enum/magic_enum.hpp"
#include "misc/cpp/imgui_stdlib.h"

#include <chrono>
#include <string>
#include <utility>

void GUI::Tabs::Debug::draw_notifications()
{
	static std::string title = "Hello";
	ImGui::InputText("Title", &title);
	static std::string message = "World";
	ImGui::InputText("Message", &message);

	static Notifications::Severity severity = Notifications::Severity::INFO;

	int temp = std::to_underlying(severity);
	ImGui::Combo("Severity", &temp, "Success\0Info\0Warning\0Error\0");
	severity = magic_enum::enum_cast<Notifications::Severity>(temp).value();

	static int max_age = 1000;
	ImGui::SliderInt("Max age", &max_age, 0, 5000);

	if (ImGui::Button("Create")) {
		Notifications::create(title, message, severity, std::chrono::milliseconds(max_age));
	}
}
