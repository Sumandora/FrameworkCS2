#include "ColorPreview.hpp"

#include "../../Setting.hpp"

#include "../../Settings/Color.hpp"
#include "imgui.h"

#include <string>
#include <utility>

ColorPreview::ColorPreview(SettingsHolder* parent, std::string name,
	Color& announce_color,
	Color& issued_by_color,
	Color& vote_cast_color,
	Color& yes_color,
	Color& no_color,
	Color& player_highlight_color)
	: Setting(parent, std::move(name))
	, announce_color(announce_color)
	, issued_by_color(issued_by_color)
	, vote_cast_color(vote_cast_color)
	, yes_color(yes_color)
	, no_color(no_color)
	, player_highlight_color(player_highlight_color)
{
}

void ColorPreview::render()
{
	// This entire thing is very helpful, but also very annoying because it is not linked to the actual thing in any way...

	ImGui::Text("Preview:");

	if (ImGui::BeginChild("##Preview", {}, ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize)) {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0F, 0.0F });

		// The other team is voting to kick player: [] (issued by [])
		ImGui::TextColored(announce_color.get_raw(), "The other team is voting to kick player: ");
		ImGui::SameLine();
		ImGui::TextColored(player_highlight_color.get_raw(), "rip_tycho");
		ImGui::SameLine();
		ImGui::TextColored(issued_by_color.get_raw(), " (issued by ");
		ImGui::SameLine();
		ImGui::TextColored(player_highlight_color.get_raw(), "rip_tycho");
		ImGui::SameLine();
		ImGui::TextColored(issued_by_color.get_raw(), ")");

		// [] voted YES
		ImGui::TextColored(player_highlight_color.get_raw(), "rip_tycho");
		ImGui::SameLine();
		ImGui::TextColored(vote_cast_color.get_raw(), " voted ");
		ImGui::SameLine();
		ImGui::TextColored(yes_color.get_raw(), "YES");

		// [] voted NO
		ImGui::TextColored(player_highlight_color.get_raw(), "rip_tycho");
		ImGui::SameLine();
		ImGui::TextColored(vote_cast_color.get_raw(), " voted ");
		ImGui::SameLine();
		ImGui::TextColored(no_color.get_raw(), "NO");

		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
}
