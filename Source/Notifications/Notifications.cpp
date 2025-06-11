#include "Notifications.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

struct Notification {
	std::string title, message;
	Notifications::Severity severity;
	std::chrono::system_clock::time_point creation_time;
	std::chrono::milliseconds max_age;

	[[nodiscard]] std::chrono::milliseconds remaining_time() const
	{
		return max_age - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - creation_time);
	}
};

static std::vector<Notification> notifications;
static std::mutex notifications_mutex;

static constexpr float NOTIFICATION_WIDTH = 200.0F;
static constexpr float NOTIFICATION_PADDING = 10.0F;

void Notifications::create(std::string title, std::string message, Severity severity, std::chrono::milliseconds max_age)
{
	const std::lock_guard guard{ notifications_mutex };
	if (!notifications.empty())
		// Only the first notification may get removed, so increase max_age accordingly
		max_age = std::max(max_age, notifications.back().remaining_time());
	notifications.emplace_back(std::move(title), std::move(message), severity, std::chrono::system_clock::now(), max_age);
}

void Notifications::render()
{
	// This lock persist during the entire time of this function, this is fine however because create is called very rarely.
	const std::lock_guard guard{ notifications_mutex };
	if (notifications.empty())
		return;

	float y = NOTIFICATION_PADDING;
	std::size_t count = 0;

	bool remove_first = false;

	for (const Notification& notification : notifications) {
		const auto current_time = std::chrono::system_clock::now();

		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - notification.creation_time);
		const float progress = static_cast<float>(elapsed.count()) / static_cast<float>(notification.max_age.count());

		// The following animation is taken from the FrameworkCSGO EventLog, more information about it can be found there.
		float animation = 1.0F - std::pow(progress / 2.0F + 0.5F, 10.0F);
		animation = std::min(animation + 0.01F, 1.0F);

		const ImVec2 display_size = ImGui::GetIO().DisplaySize;

		const std::string notification_id{ notification.title + "##Notification" + std::to_string(count) };

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, animation);
		ImGui::SetNextWindowPos({ display_size.x - NOTIFICATION_PADDING - NOTIFICATION_WIDTH, y });
		ImGui::SetNextWindowSize({ NOTIFICATION_WIDTH, 0.0F }, ImGuiCond_Once);

		switch (notification.severity) {
		case Severity::SUCESS:
			ImGui::PushStyleColor(ImGuiCol_Border, { 0.0F, 1.0F, 0.0F, 0.5F });
			break;
		case Severity::INFO:
			// No border color => normal gray outline
			break;
		case Severity::WARNING:
			ImGui::PushStyleColor(ImGuiCol_Border, { 1.0F, 1.0F, 0.0F, 0.5F });
			break;
		case Severity::ERROR:
			ImGui::PushStyleColor(ImGuiCol_Border, { 1.0F, 0.0F, 0.0F, 0.5F });
			break;
		}

		if (ImGui::Begin(notification_id.c_str(), nullptr,
				ImGuiWindowFlags_NoInputs
					| ImGuiWindowFlags_NoDecoration
					| ImGuiWindowFlags_NoMove
					| ImGuiWindowFlags_NoNav
					| ImGuiWindowFlags_NoCollapse
					| ImGuiWindowFlags_NoScrollWithMouse
					| ImGuiWindowFlags_NoFocusOnAppearing
					| ImGuiWindowFlags_NoSavedSettings)) {
			if (notification.severity != Severity::INFO)
				ImGui::PopStyleColor();

			ImGui::Text(" %s", notification.title.c_str()); // The space is separation from the circle.
			ImGui::Separator();
			ImGui::TextWrapped("%s", notification.message.c_str());
		} else if (notification.severity != Severity::INFO)
			ImGui::PopStyleColor();

		ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

		y += ImGui::GetWindowHeight() * animation;
		y += NOTIFICATION_PADDING;
		ImGui::End();
		ImGui::PopStyleVar();

		if (count == 0 && elapsed > notification.max_age)
			remove_first = true;

		count++;
	}

	if (remove_first)
		notifications.erase(notifications.begin());
}
