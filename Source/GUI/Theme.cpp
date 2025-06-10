#include "Theme.hpp"

#include "imgui.h"

static constexpr ImVec4 with_alpha(ImVec4 col, int a)
{
	ImVec4 color = col;
	color.w = static_cast<float>(a) / 255.0F;
	return color;
};

void GUI::Theme::anti_purple_theme()
{
	auto& style = ImGui::GetStyle();
	auto& colors = style.Colors;

	auto rgb = [](int r, int g, int b, int a) {
		return ImVec4(static_cast<float>(r) / 255.0F, static_cast<float>(g) / 255.0F, static_cast<float>(b) / 255.0F, static_cast<float>(a) / 255.0F);
	};

	const ImVec4 background = rgb(10, 10, 10, 255);
	const ImVec4 background2 = rgb(18, 18, 18, 255);
	const ImVec4 background3 = rgb(23, 23, 23, 255);
	const ImVec4 background31 = rgb(200, 206, 164, 255); // bg4 - 22

	// ImVec4 border = rgb(195, 204, 162, 255);
	// ImVec4 border2 = rgb(146, 155, 105, 255);
	const ImVec4 border = rgb(56, 56, 56, 255);
	const ImVec4 border2 = rgb(56, 56, 56, 255);

	const ImVec4 black = rgb(255, 255, 255, 255);
	const ImVec4 trans = rgb(0, 0, 0, 0); // :3

	colors[ImGuiCol_Text] = black;
	colors[ImGuiCol_TextDisabled] = border2;

	colors[ImGuiCol_WindowBg] = with_alpha(background, 240);
	colors[ImGuiCol_ChildBg] = trans;
	colors[ImGuiCol_PopupBg] = with_alpha(background2, 240);

	colors[ImGuiCol_Border] = with_alpha(border, 128);
	colors[ImGuiCol_BorderShadow] = trans;

	colors[ImGuiCol_FrameBg] = background3;
	colors[ImGuiCol_FrameBgHovered] = with_alpha(background31, 180);
	colors[ImGuiCol_FrameBgActive] = background31;

	colors[ImGuiCol_TitleBg] = background2;
	colors[ImGuiCol_TitleBgActive] = background2;
	colors[ImGuiCol_TitleBgCollapsed] = background2;

	colors[ImGuiCol_MenuBarBg] = background3;

	colors[ImGuiCol_ScrollbarBg] = with_alpha(background3, 100);
	colors[ImGuiCol_ScrollbarGrab] = border;
	colors[ImGuiCol_ScrollbarGrabHovered] = with_alpha(background31, 200);
	colors[ImGuiCol_ScrollbarGrabActive] = background31;

	colors[ImGuiCol_CheckMark] = background31;

	colors[ImGuiCol_SliderGrab] = border2;
	colors[ImGuiCol_SliderGrabActive] = border2;

	colors[ImGuiCol_Button] = background2;
	colors[ImGuiCol_ButtonHovered] = background3;
	colors[ImGuiCol_ButtonActive] = background2;

	colors[ImGuiCol_Header] = background2;
	colors[ImGuiCol_HeaderHovered] = with_alpha(background31, 180);
	colors[ImGuiCol_HeaderActive] = background31;

	colors[ImGuiCol_Separator] = border;
	colors[ImGuiCol_SeparatorHovered] = border;
	colors[ImGuiCol_SeparatorActive] = border;

	colors[ImGuiCol_ResizeGrip] = trans;
	colors[ImGuiCol_ResizeGripHovered] = border2;
	colors[ImGuiCol_ResizeGripActive] = border2;

	colors[ImGuiCol_Tab] = with_alpha(background3, 140);
	colors[ImGuiCol_TabHovered] = background31;
	colors[ImGuiCol_TabActive] = with_alpha(background31, 180);
	colors[ImGuiCol_TabUnfocused] = background3;
	colors[ImGuiCol_TabUnfocusedActive] = background3;

	style.WindowBorderSize = style.ChildBorderSize = style.PopupBorderSize = style.FrameBorderSize = style.TabBorderSize = 1;

	style.WindowRounding = style.ChildRounding = style.FrameRounding = style.PopupRounding = style.ScrollbarRounding = style.GrabRounding = style.TabRounding = 6;

	style.WindowTitleAlign.x = 0.5F;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	style.TabBarBorderSize = style.TabBarOverlineSize = 0;
}
