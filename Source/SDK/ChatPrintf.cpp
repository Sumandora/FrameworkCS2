#include "ChatPrintf.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "../Memory.hpp"

#include "../Utils/Logging.hpp"

#include "imgui.h"

#include <cmath>
#include <format>
#include <string>

static int (*internal_chat_printf)(void*, const char*, int, const char*) = nullptr;

void ChatPrintf::resolve_signatures()
{
	internal_chat_printf
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 49 89 CF 41 56 41 89 D6 41 55 49 89 F5 41 54 48 81 EC D0 10 00 00">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .expect<decltype(internal_chat_printf)>("Couldn't find ChatPrintf");
}

std::string ChatPrintf::to_html_rgba(const ImColor& color)
{
	return std::format("rgba({}, {}, {}, {})",
		std::floor(color.Value.x * 255),
		std::floor(color.Value.y * 255),
		std::floor(color.Value.z * 255),
		color.Value.w);
}

void ChatPrintf::print(const std::string& text)
{
	if (internal_chat_printf) {
		internal_chat_printf(nullptr, "\x01" /*Enable html markup*/, -1 /* team filter? */, text.c_str());
		return;
	}

	// The function wasn't found, lets print to terminal
	Logging::info("{}", text.c_str());
}
