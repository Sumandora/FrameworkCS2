#include "Theme.hpp"

#include "imgui.h"

static constexpr ImColor with_alpha(ImColor color, int a)
{
	color.Value.w = static_cast<float>(a) / 255.0F;
	return color;
};

void GUI::Theme::anti_purple_theme()
{
	auto& style = ImGui::GetStyle();
	auto& colors = style.Colors;

	const ImColor background = IM_COL32(10, 10, 10, 255);
	const ImColor background2 = IM_COL32(18, 18, 18, 255);
	const ImColor background3 = IM_COL32(23, 23, 23, 255);

	const ImColor accent = IM_COL32(200, 206, 164, 255);

	const ImColor border = IM_COL32(56, 56, 56, 255);
	const ImColor border2 = IM_COL32(56, 56, 56, 255);

	const ImColor white = IM_COL32_WHITE;
	const ImColor trans = IM_COL32_BLACK_TRANS; // :3

	colors[ImGuiCol_Text] = white;
	colors[ImGuiCol_TextDisabled] = border2;

	colors[ImGuiCol_WindowBg] = with_alpha(background, 240);
	colors[ImGuiCol_ChildBg] = trans;
	colors[ImGuiCol_PopupBg] = with_alpha(background2, 240);

	colors[ImGuiCol_Border] = with_alpha(border, 128);
	colors[ImGuiCol_BorderShadow] = trans;

	colors[ImGuiCol_FrameBg] = background3;
	colors[ImGuiCol_FrameBgHovered] = with_alpha(accent, 180);
	colors[ImGuiCol_FrameBgActive] = accent;

	colors[ImGuiCol_TitleBg] = background2;
	colors[ImGuiCol_TitleBgActive] = background2;
	colors[ImGuiCol_TitleBgCollapsed] = background2;

	colors[ImGuiCol_MenuBarBg] = background3;

	colors[ImGuiCol_ScrollbarBg] = with_alpha(background3, 100);
	colors[ImGuiCol_ScrollbarGrab] = border;
	colors[ImGuiCol_ScrollbarGrabHovered] = with_alpha(accent, 200);
	colors[ImGuiCol_ScrollbarGrabActive] = accent;

	colors[ImGuiCol_CheckMark] = accent;

	colors[ImGuiCol_SliderGrab] = border2;
	colors[ImGuiCol_SliderGrabActive] = border2;

	colors[ImGuiCol_Button] = background2;
	colors[ImGuiCol_ButtonHovered] = background3;
	colors[ImGuiCol_ButtonActive] = background2;

	colors[ImGuiCol_Header] = background2;
	colors[ImGuiCol_HeaderHovered] = with_alpha(accent, 180);
	colors[ImGuiCol_HeaderActive] = accent;

	colors[ImGuiCol_Separator] = border;
	colors[ImGuiCol_SeparatorHovered] = border;
	colors[ImGuiCol_SeparatorActive] = border;

	colors[ImGuiCol_ResizeGrip] = trans;
	colors[ImGuiCol_ResizeGripHovered] = border2;
	colors[ImGuiCol_ResizeGripActive] = border2;

	colors[ImGuiCol_Tab] = with_alpha(background3, 140);
	colors[ImGuiCol_TabHovered] = accent;
	colors[ImGuiCol_TabActive] = with_alpha(accent, 180);
	colors[ImGuiCol_TabUnfocused] = background3;
	colors[ImGuiCol_TabUnfocusedActive] = background3;

	style.WindowBorderSize = style.ChildBorderSize = style.PopupBorderSize = style.FrameBorderSize = style.TabBorderSize = 2;

	style.WindowRounding = style.ChildRounding = style.FrameRounding = style.PopupRounding = style.ScrollbarRounding = style.GrabRounding = style.TabRounding = 6;

	style.WindowTitleAlign.x = 0.5F;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	style.TabBarBorderSize = style.TabBarOverlineSize = 0;

	style.FramePadding.x = 10;
}
