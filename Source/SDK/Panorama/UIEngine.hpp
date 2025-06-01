#pragma once

#include "../Padding.hpp"
#include "../VirtualMethod.hpp"

#include "UIPanel.hpp"

#include "../CUtl/RBTree.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>

struct UIEngine /* a.k.a CUIEngine */ {
private:
	PADDING(0x1a8);

public:
	UtlRBTree<UIPanel*> panels;

	VIRTUAL_METHOD(32, is_valid_panel, bool, (UIPanel * panel), (this, panel));
	VIRTUAL_METHOD(80, run_script, void, (UIPanel * panel, const char* source_code, const char* context, int64_t unk2), (this, panel, source_code, context, unk2));

	// TODO actually traverse this data structure
	template <typename F>
		requires std::invocable<F, UIPanel*>
	void for_each_panel(const F& f)
	{
		for (int i = 0; i < panels.elements_count; i++) {
			UIPanel* p = *reinterpret_cast<UIPanel**>(&panels.memory.memory[i]);
			if (is_valid_panel(p)) {
				f(p);
			}
		}
	}

	[[nodiscard]] UIPanel* find_panel(std::string_view id)
	{
		for (int i = 0; i < panels.elements_count; i++) {
			UIPanel* p = *reinterpret_cast<UIPanel**>(&panels.memory.memory[i]);
			if (is_valid_panel(p) &&  p->id && p->id == id) {
				return p;
			}
		}
		return nullptr;
	}

	void run_script(UIPanel* panel, const char* source_code, const char* context = "")
	{
		run_script(panel, source_code, context, 1);
	}
};
