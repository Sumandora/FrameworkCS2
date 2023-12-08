#include "Menu.hpp"

#include "../Config/Config.hpp"
#include "Components/Window.hpp"
#include "Fonts/fontawesome.hpp"
#include "Fonts/smallestpixel7.hpp"
#include "Fonts/verdana.hpp"
#include "Fonts/verdana_bold.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#include <backends/imgui_impl_vulkan.h>

Menu::Window* window;

ImVec4 makeTransparent(ImVec4 color, const float transparency)
{
	color.w = transparency;
	return color;
}

ImVec4 accentColorTransparent(const float transparency)
{
	return makeTransparent(ImGui::ColorConvertU32ToFloat4(*Config::c.menu.accentColor), transparency);
}

bool fontsNeedUpdate = true; // for the first load

void updateFontDPIImpl()
{
	if (!fontsNeedUpdate) {
		return;
	}

	static const ImWchar icons_ranges[] = { 0xe005, 0xf8ff, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	const auto io = ImGui::GetIO();

	const float fourteen = static_cast<int>(14.0f * Config::getDpiScale());

	io.Fonts->Clear();
	Menu::fonts.menuDefault
		= io.Fonts->AddFontFromMemoryCompressedTTF(Verdana_compressed_data, Verdana_compressed_size, fourteen);
	io.Fonts->AddFontFromMemoryCompressedTTF(
		FontAwesome_compressed_data, FontAwesome_compressed_size, fourteen, &icons_config, icons_ranges);
	Menu::fonts.menuBold
		= io.Fonts->AddFontFromMemoryCompressedTTF(VerdanaBold_compressed_data, VerdanaBold_compressed_size, fourteen);
	Menu::fonts.menuBig = io.Fonts->AddFontFromMemoryCompressedTTF(
		VerdanaBold_compressed_data, VerdanaBold_compressed_size, static_cast<int>(18.0f * Config::getDpiScale()));

	Menu::fonts.espDefault
		= io.Fonts->AddFontFromMemoryCompressedTTF(Verdana_compressed_data, Verdana_compressed_size, 12.0f);
	Menu::fonts.espBold
		= io.Fonts->AddFontFromMemoryCompressedTTF(VerdanaBold_compressed_data, VerdanaBold_compressed_size, 12.0f);
	Menu::fonts.espSmall = io.Fonts->AddFontFromMemoryCompressedTTF(
		SmallestPixel7_compressed_data, SmallestPixel7_compressed_size, 10.0f);

	Menu::espFonts[0] = Menu::fonts.espDefault;
	Menu::espFonts[1] = Menu::fonts.espBold;
	Menu::espFonts[2] = Menu::fonts.espSmall;

	fontsNeedUpdate = false;
}

namespace Menu {
	void init()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Don't create imgui.ini
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;

		updateFontDPIImpl();

		ImGui::GetStyle().FramePadding = ImVec2(4, 2);
	}

	void draw()
	{
		const float windowScale = Config::getDpiScale();

		auto& style = ImGui::GetStyle();
		style.WindowRounding = 8.0f * windowScale;
		style.ChildRounding = 8.0f * windowScale;
		style.FrameRounding = 4.0f * windowScale;
		style.PopupRounding = 8.0f * windowScale;
		style.WindowPadding.x = 8.0f * windowScale;

		ImVec4* colors = style.Colors;

		const ImVec4 accentColor = ImGui::ColorConvertU32ToFloat4(*Config::c.menu.accentColor);
		const ImVec4 halfAccentColor = ImLerp(ImVec4(0.0f, 0.0f, 0.0f, 0.0f), accentColor, 0.5f);

		// based on the dark imgui theme
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImLerp(ImVec4(0.4f, 0.4f, 0.4f, 0.4f), accentColor, 0.1f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = halfAccentColor;
		colors[ImGuiCol_FrameBgHovered] = accentColorTransparent(0.4f);
		colors[ImGuiCol_FrameBgActive] = accentColorTransparent(0.67f);
		colors[ImGuiCol_TitleBg] = makeTransparent(halfAccentColor, 0.9f);
		colors[ImGuiCol_TitleBgActive] = makeTransparent(halfAccentColor, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = accentColor;
		colors[ImGuiCol_SliderGrab] = ImLerp(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), accentColor, 0.9f);
		colors[ImGuiCol_SliderGrabActive] = accentColor;
		colors[ImGuiCol_Button] = makeTransparent(accentColor, 0.4f);
		colors[ImGuiCol_ButtonHovered] = accentColor;
		colors[ImGuiCol_ButtonActive] = makeTransparent(accentColor, 0.7f);
		colors[ImGuiCol_Header] = makeTransparent(accentColor, 0.31f);
		colors[ImGuiCol_HeaderHovered] = makeTransparent(accentColor, 0.8f);
		colors[ImGuiCol_HeaderActive] = accentColor;
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImLerp(ImVec4(0.0f, 0.0f, 0.0f, 0.5f), accentColor, 0.3f);
		colors[ImGuiCol_SeparatorActive] = ImLerp(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), accentColor, 0.3f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0, 0, 0, 0);
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f); // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f); // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = makeTransparent(accentColor, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = accentColor;
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		if (isOpen) {
			window.draw();
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			isOpen = !isOpen;
		}

		// Features::Misc::Menu::draw();

		auto io = ImGui::GetIO();

		io.MouseDrawCursor = isOpen;
		io.WantCaptureMouse = isOpen;
		io.WantCaptureKeyboard = isOpen;
	}

	void updateFontDPI() { fontsNeedUpdate = true; }

	void postRender()
	{
		updateFontDPIImpl();
		ImGui_ImplVulkan_CreateFontsTexture();
	}
}