#pragma once

#include "../Padding.hpp"

#include "cs_usercmd.pb.h" // IWYU pragma: export
#include "network_connection.pb.h" // IWYU pragma: export
#include "networkbasetypes.pb.h" // IWYU pragma: export
#include "usercmd.pb.h" // IWYU pragma: export

#include <cstdint>

struct BasePlayerController;

static constexpr std::uint64_t IN_JUMP = 1 << 1;
static constexpr std::uint64_t IN_FORWARD = 1 << 3;
static constexpr std::uint64_t IN_BACK = 1 << 4;
static constexpr std::uint64_t IN_MOVELEFT = 1 << 9;
static constexpr std::uint64_t IN_MOVERIGHT = 1 << 10;

struct Buttons {
	PADDING(0x8);
	std::uint64_t buttonstate1;
	std::uint64_t buttonstate2;
	std::uint64_t buttonstate3;
};

struct UserCmd {
private:
	PADDING(0x10); // uh...

public:
	// NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
	CSGOUserCmdPB csgo_usercmd;
	Buttons buttons;
	// NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

private:
	PADDING(0x10);

public:
	// NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
	bool has_been_predicted;

private:
	PADDING(0xF);

public:
	static void resolve_signatures();
	static UserCmd* get_current_command(BasePlayerController* controller);

	CSubtickMoveStep* allocate_new_move_step(float when);

	// Sets button state inside Buttons and in CInButtonStatePB
	void set_buttonstate1(std::uint64_t value);
	void set_buttonstate2(std::uint64_t value);
	void set_buttonstate3(std::uint64_t value);

	// Takes forwardmove and leftmove and applies button inputs accordingly
	void fixup_buttons_for_move();

	void fixup_button_changes(const Buttons& old_buttons);
};

static_assert(sizeof(UserCmd) == 0x98);
