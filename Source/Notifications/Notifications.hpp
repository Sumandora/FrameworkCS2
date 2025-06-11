#pragma once

#include "imgui.h"
#include <chrono>
#include <cstdint>
#include <ctime>
#include <string>

struct ImDrawList;

namespace Notifications {
	enum class Severity : std::uint8_t {
		SUCESS,
		INFO,
		WARNING,
		ERROR,
	};


	void create(std::string title, std::string message, Severity severity, std::chrono::milliseconds max_age = std::chrono::seconds(1));

	void render();
}
