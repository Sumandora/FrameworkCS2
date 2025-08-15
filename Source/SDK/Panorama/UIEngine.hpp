#pragma once

#include "../Padding.hpp"
#include "../VirtualMethod.hpp"

#include "UIPanel.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>

struct UIEngine /* a.k.a CUIEngine */ {
private:
	struct PanelNode {
		PADDING(0x10);
		UIPanel* panel;
		PADDING(0x8);
	};
	static_assert(sizeof(PanelNode) == 0x20);
	OFFSET(int, count, 0x310)
	OFFSET(PanelNode*, panels, 0x308);

public:
	VIRTUAL_METHOD(32, is_valid_panel, bool, (UIPanel * panel), (this, panel));
	VIRTUAL_METHOD(78, run_script, void, (UIPanel * panel, const char* source_code, const char* context, int64_t unk2), (this, panel, source_code, context, unk2));

	// TODO actually traverse this data structure
	template <typename F>
		requires std::invocable<F, UIPanel*>
	void for_each_panel(const F& f)
	{
		for (int i = 0; i < count(); i++) {
			PanelNode& p = panels()[i];
			if (is_valid_panel(p.panel)) {
				f(p.panel);
			}
		}
	}

	[[nodiscard]] UIPanel* find_panel(std::string_view id)
	{
		for (int i = 0; i < count(); i++) {
			const PanelNode& p = panels()[i];
			if (is_valid_panel(p.panel) && p.panel->id && p.panel->id == id) {
				return p.panel;
			}
		}
		return nullptr;
	}

	void run_script(UIPanel* panel, const char* source_code, const char* context = "")
	{
		run_script(panel, source_code, context, 1);
	}
};
