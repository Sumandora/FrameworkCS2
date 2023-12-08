#include "ButtonArray.hpp"

#include "../../Config/Config.hpp"

#include <imgui.h>

namespace Menu {
	ButtonArray::ButtonArray(std::vector<std::string> names, const bool highlightSelected,
		std::optional<std::function<void(std::size_t)>> callback, const std::optional<float> width)
		: names(std::move(names))
		, highlightSelected(highlightSelected)
		, callback(std::move(callback))
		, width(width)
	{
	}

	ButtonArray::ButtonArray(std::vector<std::string> names, const std::optional<float> width)
		: ButtonArray(std::move(names), true, std::nullopt, width)
	{
	}

	ButtonArray::ButtonArray(
		const std::vector<std::pair<std::string, std::function<void()>>>& actions, const std::optional<float> width)
	{
		std::vector<std::function<void()>> callbacks;
		names.reserve(actions.size());
		callbacks.reserve(actions.size());
		for (const auto& [name, callback] : actions) {
			names.push_back(name);
			callbacks.push_back(callback);
		}
		callback = [callbacks = std::move(callbacks)](const std::size_t selected) { callbacks[selected](); };
		highlightSelected = false;
		this->width = width;
	}

	ButtonArray::ButtonArray(
		std::vector<std::string> names, std::function<void(std::size_t)> callback, const std::optional<float> width)
		: ButtonArray(std::move(names), false, std::move(callback), width)
	{
	}

	void ButtonArray::draw()
	{
		const float totalWidth
			= width ? *width * Config::getDpiScale() * names.size() : ImGui::GetContentRegionAvail().x;
		if (width) { // right-align
			ImGui::SetCursorPosX(std::round(ImGui::GetContentRegionMax().x - totalWidth));
		}

		// draw background rect and outline
		auto screenPos = ImGui::GetCursorScreenPos();
		const auto drawList = ImGui::GetWindowDrawList();
		const float top = screenPos.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2;
		drawList->AddRectFilled(screenPos, ImVec2(screenPos.x + totalWidth, top), ImGui::GetColorU32(ImGuiCol_FrameBg),
			ImGui::GetStyle().FrameRounding);
		drawList->AddRect(screenPos, ImVec2(screenPos.x + totalWidth, top), ImGui::GetColorU32(ImGuiCol_Border),
			ImGui::GetStyle().FrameRounding);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0, 0, 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0, 0, 0, 0 });

		float buttonWidth = totalWidth / names.size();
		auto [cursorX, cursorY] = ImGui::GetMousePos();
		std::optional<std::size_t> hoveredButton;
		if (cursorY >= screenPos.y && cursorY <= top) {
			if (cursorX >= screenPos.x && cursorX <= screenPos.x + totalWidth) {
				hoveredButton = static_cast<std::size_t>((cursorX - screenPos.x) / buttonWidth);
			}
		}

		for (std::size_t i = 0; i < names.size(); ++i) {
			if (i != 0) {
				ImGui::SameLine();
			}
			screenPos = ImGui::GetCursorScreenPos();
			std::optional<ImColor> buttonColor;
			if (highlightSelected && selected == i) {
				buttonColor = ImGui::GetColorU32(ImGuiCol_FrameBgActive);
			} else if (hoveredButton && *hoveredButton == i) {
				buttonColor = ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
			}
			if (buttonColor) {
				ImDrawFlags flags = ImDrawFlags_RoundCornersNone;
				if (i == 0) {
					flags = ImDrawFlags_RoundCornersLeft;
				} else if (i == names.size() - 1) {
					flags = ImDrawFlags_RoundCornersRight;
				}
				drawList->AddRectFilled(screenPos, { screenPos.x + buttonWidth, top }, *buttonColor,
					ImGui::GetStyle().FrameRounding, flags);
			}
			if (i != 0) {
				const float lineHeight = (top - screenPos.y) / 2;
				drawList->AddLine({ std::floor(screenPos.x), std::floor(screenPos.y + lineHeight / 2) },
					{ std::floor(screenPos.x), std::floor(top - lineHeight / 2) }, ImGui::GetColorU32(ImGuiCol_Border));
			}
			if (ImGui::Button(names[i].c_str(), { buttonWidth, 0 })) {
				selected = i;
				if (callback) {
					(*callback)(i);
				}
			}
		}

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
	}

	std::size_t ButtonArray::getSelected() const { return selected; }

	void ButtonArray::addButton(std::string name) { names.push_back(std::move(name)); }
}