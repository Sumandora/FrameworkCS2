#pragma once

#include "../Padding.hpp"

#include "cs_usercmd.pb.h" // IWYU pragma: export
#include "network_connection.pb.h" // IWYU pragma: export
#include "networkbasetypes.pb.h" // IWYU pragma: export
#include "usercmd.pb.h" // IWYU pragma: export

#include <cstdint>

struct BasePlayerController;

// @schema InputBitMask_t
enum InputBitMask : std::uint64_t
{
	IN_NONE = 0x0,
	IN_ALL = 0xffffffffffffffff,
	IN_ATTACK = 1UL << 0UL,
	IN_JUMP = 1UL << 1UL,
	IN_DUCK = 1UL << 2UL,
	IN_FORWARD = 1UL << 3UL,
	IN_BACK = 1UL << 4UL,
	IN_USE = 1UL << 5UL,
	IN_TURNLEFT = 1UL << 7UL,
	IN_TURNRIGHT = 1UL << 8UL,
	IN_MOVELEFT = 1UL << 9UL,
	IN_MOVERIGHT = 1UL << 10UL,
	IN_ATTACK2 = 1UL << 11UL,
	IN_RELOAD = 1UL << 13UL,
	IN_SPEED = 1UL << 16UL,
	IN_JOYAUTOSPRINT = 1UL << 17UL,
	IN_FIRST_MOD_SPECIFIC_BIT = 1UL << 32UL,
	IN_USEORRELOAD = 1UL << 32UL,
	IN_SCORE = 1UL << 33UL,
	IN_ZOOM = 1UL << 34UL,
	IN_LOOK_AT_WEAPON = 1UL << 35UL,
};

static constexpr int MAX_SUBTICKS_PER_MOVE = 12;

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
