#include "../Tabs.hpp"
#include "imgui.h"

void GUI::Tabs::About::draw_licenses()
{
#define CONCAT(a, b) a##b
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONCAT2(a, b) CONCAT(a, b)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LICENSE(pretty_name, name)                  \
	extern char CONCAT2(name, _license)[];          \
	if (ImGui::CollapsingHeader(pretty_name)) {     \
		ImGui::Text("%s", CONCAT2(name, _license)); \
	}
	ITERATE_LICENSES;
}
