#include "ColorPicker.hpp"

#include <format>

namespace Menu {
	ColorPicker::ColorPicker(std::string name, Config::Setting<ImU32>& setting, bool noAlpha)
		: Widget(std::move(name))
		, mainPopupLabel(getLabel() + "mainPopup")
		, setting(setting)
		, noAlpha(noAlpha)
		, copyPasteButtons("Copy/paste",
			  { { "\xef\x83\x85 Copy", [this] {
					 const auto color = *this->setting.getPointer();
					 ImGui::SetClipboardText(std::format("#{:08x}", color).c_str());
				 } },
				  { "\xef\x83\xaa Paste", [this] {
					   const auto clipboardText = ImGui::GetClipboardText();
					   if (!clipboardText || clipboardText[0] != '#')
						   return;
					   const auto color = std::strtoul(clipboardText + 1, nullptr, 16);
					   *this->setting.getPointer() = color;
				   } } })
	{
	}

	void ColorPicker::draw()
	{
		ImGui::TextUnformatted(name.c_str());
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - width() - ImGui::GetStyle().ItemSpacing.x);
		drawInline();
	}

	void ColorPicker::drawInline()
	{
		auto colorVec = ImGui::ColorConvertU32ToFloat4(*setting.getPointer());
		if (ImGui::ColorButton(getLabel().c_str(), colorVec,
				ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview)) {
			ImGui::OpenPopup(mainPopupLabel.c_str());
		}
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 8, 4 });
		if (ImGui::BeginPopupContextItem()) {
			copyPasteButtons.draw();
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(1);
		if (ImGui::BeginPopup(mainPopupLabel.c_str())) {
			// TODO: maybe add support for dynamic colors, i.e.
			// - colors that oscillate between two values
			// - rainbow colors
			// see primordial for an example
			ImGui::ColorPicker4("##picker", reinterpret_cast<float*>(&colorVec),
				(noAlpha ? 0 : ImGuiColorEditFlags_AlphaBar) | ImGuiColorEditFlags_NoSidePreview
					| ImGuiColorEditFlags_NoSmallPreview & ~ImGuiColorEditFlags_Uint8);
			*setting.getPointer() = ImGui::ColorConvertFloat4ToU32(colorVec);
			ImGui::EndPopup();
		}
	}

	float ColorPicker::width() { return ImGui::GetFontSize(); }
}