#pragma once

#include "../Padding.hpp"

struct UIPanel {
private:
	PADDING(0x10);

public:
	const char* id;
};
