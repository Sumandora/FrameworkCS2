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

static int (*internal_chat_printf)(void*, const char*, int, const char*, ...) __attribute__((format(printf, 4, 5))) = nullptr;

void ChatPrintf::resolve_signatures()
{
	// TODO Use the string inside the function
	internal_chat_printf
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 56 49 89 CE 41 55 41 89 D5 41 54 49 89 F4 53 48 81 EC D0 10 00 00">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .BCRL_EXPECT(decltype(internal_chat_printf), internal_chat_printf);
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
		internal_chat_printf(nullptr, "\x01" /*Enable html markup*/, -1 /* team filter? */, "%s", text.c_str());
		return;
	}

	// The function wasn't found, lets print to terminal
	Logging::info("{}", text.c_str());
}
