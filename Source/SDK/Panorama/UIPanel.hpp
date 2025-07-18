#pragma once

#include "../Padding.hpp"
#include "../VirtualMethod.hpp"

#include <cstdint>

enum PanelFlags : std::uint8_t {
	VISIBLE = 1 << 0,
};

struct UIPanel {
private:
	PADDING(0x10);

public:
	const char* id;

private:
	PADDING(0x10c);

public:
	PanelFlags flags;

	VIRTUAL_METHOD(35, is_visible, bool, (), (this));
};
