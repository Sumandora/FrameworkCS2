#pragma once

#include <array>

#include "Components/Window.hpp"
#include "imgui.h"

namespace Menu {
    void init();
    void uninit();

	void draw();

    inline Window window{"l8r"};
	inline bool isOpen = true;

	void updateFontDPI();

	void postRender();

    struct Fonts {
        ImFont* menuDefault;
        ImFont* menuBig;
        ImFont* menuBold;
        ImFont* espDefault;
        ImFont* espSmall;
        ImFont* espBold;
    };

    inline Fonts fonts;
	// for access by enum in ESP code
    inline std::array<ImFont*, 3> espFonts;
}  // namespace Menu