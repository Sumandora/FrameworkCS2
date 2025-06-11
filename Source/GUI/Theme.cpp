#include "Theme.hpp"

#include "imgui.h"
#include "imnodes.h"

static constexpr ImColor with_alpha(ImColor color, int a)
{
	color.Value.w = static_cast<float>(a) / 255.0F;
	return color;
};

static constexpr ImColor BACKGROUND = IM_COL32(10, 10, 10, 255);
static constexpr ImColor BACKGROUND2 = IM_COL32(18, 18, 18, 255);
static constexpr ImColor BACKGROUND3 = IM_COL32(23, 23, 23, 255);

static constexpr ImColor ACCENT = IM_COL32(200, 206, 164, 255);

static constexpr ImColor BORDER = IM_COL32(56, 56, 56, 255);
static constexpr ImColor BORDER2 = IM_COL32(56, 56, 56, 255);

static constexpr ImColor WHITE = IM_COL32_WHITE;
static constexpr ImColor TRANS = IM_COL32_BLACK_TRANS; // :3

void GUI::Theme::anti_purple_theme()
{
	auto& style = ImGui::GetStyle();
	auto& colors = style.Colors;

	colors[ImGuiCol_Text] = WHITE;
	colors[ImGuiCol_TextDisabled] = BORDER2;

	colors[ImGuiCol_WindowBg] = with_alpha(BACKGROUND, 240);
	colors[ImGuiCol_ChildBg] = TRANS;
	colors[ImGuiCol_PopupBg] = with_alpha(BACKGROUND, 240);

	colors[ImGuiCol_Border] = with_alpha(BORDER, 128);
	colors[ImGuiCol_BorderShadow] = TRANS;

	colors[ImGuiCol_FrameBg] = BACKGROUND3;
	colors[ImGuiCol_FrameBgHovered] = with_alpha(ACCENT, 180);
	colors[ImGuiCol_FrameBgActive] = ACCENT;

	colors[ImGuiCol_TitleBg] = BACKGROUND2;
	colors[ImGuiCol_TitleBgActive] = BACKGROUND2;
	colors[ImGuiCol_TitleBgCollapsed] = BACKGROUND2;

	colors[ImGuiCol_MenuBarBg] = BACKGROUND3;

	colors[ImGuiCol_ScrollbarBg] = with_alpha(BACKGROUND3, 100);
	colors[ImGuiCol_ScrollbarGrab] = BORDER;
	colors[ImGuiCol_ScrollbarGrabHovered] = with_alpha(ACCENT, 200);
	colors[ImGuiCol_ScrollbarGrabActive] = ACCENT;

	colors[ImGuiCol_CheckMark] = ACCENT;

	colors[ImGuiCol_SliderGrab] = BORDER2;
	colors[ImGuiCol_SliderGrabActive] = BORDER2;

	colors[ImGuiCol_Button] = BACKGROUND2;
	colors[ImGuiCol_ButtonHovered] = BACKGROUND3;
	colors[ImGuiCol_ButtonActive] = BACKGROUND2;

	colors[ImGuiCol_Header] = BACKGROUND2;
	colors[ImGuiCol_HeaderHovered] = with_alpha(ACCENT, 180);
	colors[ImGuiCol_HeaderActive] = ACCENT;

	colors[ImGuiCol_Separator] = BORDER;
	colors[ImGuiCol_SeparatorHovered] = BORDER;
	colors[ImGuiCol_SeparatorActive] = BORDER;

	colors[ImGuiCol_ResizeGrip] = TRANS;
	colors[ImGuiCol_ResizeGripHovered] = BORDER2;
	colors[ImGuiCol_ResizeGripActive] = BORDER2;

	colors[ImGuiCol_Tab] = with_alpha(BACKGROUND3, 140);
	colors[ImGuiCol_TabHovered] = ACCENT;
	colors[ImGuiCol_TabActive] = with_alpha(ACCENT, 180);
	colors[ImGuiCol_TabUnfocused] = BACKGROUND3;
	colors[ImGuiCol_TabUnfocusedActive] = BACKGROUND3;

	style.WindowBorderSize = style.ChildBorderSize = style.PopupBorderSize = style.FrameBorderSize = style.TabBorderSize = 2;

	style.WindowRounding = style.ChildRounding = style.FrameRounding = style.PopupRounding = style.ScrollbarRounding = style.GrabRounding = style.TabRounding = 6;

	style.WindowTitleAlign.x = 0.5F;
	style.WindowMenuButtonPosition = ImGuiDir_None;

	style.TabBarBorderSize = style.TabBarOverlineSize = 0;

	style.FramePadding.x = 10;
}

void GUI::Theme::anti_purple_imnodes_theme()
{
	auto& colors = ImNodes::GetStyle().Colors;

	colors[ImNodesCol_TitleBar] = with_alpha(ACCENT, 140);
	colors[ImNodesCol_TitleBarHovered] = with_alpha(ACCENT, 180);
	colors[ImNodesCol_TitleBarSelected] = ACCENT;

	colors[ImNodesCol_Link] = with_alpha(ACCENT, 140);
	colors[ImNodesCol_LinkHovered] = with_alpha(ACCENT, 180);
	colors[ImNodesCol_LinkSelected] = ACCENT;

	colors[ImNodesCol_Pin] = with_alpha(ACCENT, 180);
	colors[ImNodesCol_PinHovered] = ACCENT;

	colors[ImNodesCol_BoxSelector] = with_alpha(ACCENT, 30);
	colors[ImNodesCol_BoxSelectorOutline] = with_alpha(ACCENT, 150);

	colors[ImNodesCol_GridBackground] = TRANS;
	colors[ImNodesCol_GridLine] = with_alpha(WHITE, 40);

	colors[ImNodesCol_MiniMapLink] = colors[ImNodesCol_Link];
	colors[ImNodesCol_MiniMapLinkSelected] = colors[ImNodesCol_LinkSelected];
}
