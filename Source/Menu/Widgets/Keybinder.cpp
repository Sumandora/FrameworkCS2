#include "Keybinder.hpp"

#include "../../Config/Key.hpp"

#include <imgui.h>
#include <optional>
#include <utility>

std::optional<Config::Key> applyKeypress()
{
	for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); i++) {
		if (ImGui::IsKeyDown(static_cast<ImGuiKey>(i))) {
			if (i == ImGuiKey_Escape) {
				return Config::Key::disabled();
			}
			return Config::Key::fromKey(static_cast<ImGuiKey>(i));
		}
	}
	const auto* buttons = ImGui::GetIO().MouseDown;
	for (int i = 0; i < 5; i++) {
		if (buttons[i]) {
			return Config::Key::fromMouseButton(i);
		}
	}
	return std::nullopt;
}

namespace Menu {
	Keybinder::Keybinder(std::string label, Config::Key* key)
		: label(std::move(label))
		, key(key)
	{
	}

	void Keybinder::draw(const std::string& label, Config::Key& key)
	{
		static const char* waitingFor = nullptr;
		const bool isWaitingForThis = waitingFor == label.c_str();
		if (isWaitingForThis) {
			if (const auto keyPressed = applyKeypress()) {
				waitingFor = nullptr;
				key = *keyPressed;
			}
		}

		std::string_view currentLabel;
		static constexpr std::string_view waiting = "Press a key...";
		if (isWaitingForThis) {
			currentLabel = waiting;
		} else {
			currentLabel = key.toString();
		}

		const auto yPadding = ImGui::GetStyle().FramePadding.y;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yPadding);
		ImGui::TextUnformatted(label.c_str());
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(currentLabel.data()).x
			- 2 * ImGui::GetStyle().FramePadding.x);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - yPadding);
		if (ImGui::Button(currentLabel.data())) {
			waitingFor = label.c_str();
		}
	}

	void Keybinder::draw() { draw(label, *key); }
}