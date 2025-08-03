#include "ChatPrintf.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "../Memory.hpp"

#include "../Utils/Logging.hpp"

#include "Padding.hpp"

#include "imgui.h"

#include <cmath>
#include <format>
#include <string>

static int (*internal_chat_printf)(void*, const char*, int, const char*, ...) __attribute__((format(printf, 4, 5))) = nullptr;

struct LocalPlayerFilter;

static void (*init_local_player_filter)(LocalPlayerFilter*) = nullptr;
// Not the same EmitSound as the hook, this requires precise parameters
static void (*emit_sound)(float, float, void*, LocalPlayerFilter*, int, const char*, int, int) = nullptr;

struct LocalPlayerFilter {
	PADDING(40);

	LocalPlayerFilter()
	{
		init_local_player_filter(this);
	}
};

void ChatPrintf::resolve_signatures()
{
	// TODO Use the string inside the function
	internal_chat_printf
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 56 49 89 CE 41 55 41 89 D5 41 54 49 89 F4 53 48 81 EC D0 10 00 00">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .finalize<decltype(internal_chat_printf)>()
			  .value_or(nullptr);

	MEM_ACCEPT(internal_chat_printf);

	init_local_player_filter
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"18CLocalPlayerFilter">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::absolute(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .sub(8)
			  .find_xrefs(SignatureScanner::XRefTypes::absolute(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .add(8)
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 8d 05">())
			  .finalize<decltype(init_local_player_filter)>()
			  .value_or(nullptr);

	MEM_ACCEPT(init_local_player_filter);

	emit_sound
		= (decltype(emit_sound))BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"HudChat.Message">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .sub(3)
			  .next_instruction()
			  .add(1)
			  .relative_to_absolute()
			  // .BCRL_EXPECT(decltype(emit_sound), emit_sound);
			  .peek()
			  .front()
			  .get_pointer();

	MEM_ACCEPT(emit_sound);
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

		if (init_local_player_filter && emit_sound) {
			char out[32];
			LocalPlayerFilter filter{};
			emit_sound(0.0F, 1.0F, &out, &filter, -1, "HudChat.Message", 0, 0);
		}
		return;
	}

	// The function wasn't found, lets print to terminal
	Logging::info("{}", text);
}
