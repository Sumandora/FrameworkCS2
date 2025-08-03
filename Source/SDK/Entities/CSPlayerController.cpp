#include "CSPlayerController.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "../../Memory.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ranges>
#include <string>

namespace DecoratedPlayerNameFlag {
	enum : std::uint8_t {
		SIMPLE = 0,
		ADD_BOT_TO_NAME_IF_CONTROLLING_BOT = 1 << 0,
		DONT_USE_NAME_OF_CONTROLLING_PLAYER = 1 << 1,
		DONT_SHOW_CLAN_NAME = 1 << 2,
		DONT_MAKE_STRING_SAFE = 1 << 3,
		DONT_USE_ASSASSINATION_TARGET_NAME = 1 << 4,
		UNKNOWN = 1 << 5, // I don't know what this does
	};
}

static const char* (*get_decorated_player_name)(const CSPlayerController* controller, char* buffer, int bufsize, std::uint8_t flags) = nullptr;

void CSPlayerController::resolve_signatures()
{
	::get_decorated_player_name
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"55 89 C8 48 89 E5 41 57 83 E0 20">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .BCRL_EXPECT(decltype(::get_decorated_player_name), ::get_decorated_player_name);
}

std::string CSPlayerController::get_decorated_player_name() const
{
	// Not sure if this is the actual max length, but it once was.
	static constexpr std::size_t MAX_NETWORKID_LENGTH = 64;
	static constexpr std::size_t MAX_DECORATED_PLAYER_NAME_LENGTH = MAX_NETWORKID_LENGTH * 10 + 20;

	char chars[MAX_DECORATED_PLAYER_NAME_LENGTH];
	::get_decorated_player_name(this, chars, std::ranges::size(chars), DecoratedPlayerNameFlag::DONT_MAKE_STRING_SAFE);

	std::string s{ std::ranges::begin(chars), std::strlen(chars) };

	// Some maps try to be funny by adding \r characters into bot names, notably aim_botz.
	std::erase(s, '\r');

	return s;
}
