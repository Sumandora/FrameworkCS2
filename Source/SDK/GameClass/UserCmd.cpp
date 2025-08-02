#include "UserCmd.hpp"

#include "../GameClass/Source2Client.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../Interfaces.hpp"
#include "../../Memory.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>

static void** usercmd_list = nullptr;
static int (*cmdlist_index_from_entity)(BasePlayerController*) = nullptr;
static void* (*get_usercmds)(void*, uint64_t) = nullptr;
static UserCmd* (*get_usercmd)(BasePlayerController*, int) = nullptr;
static CSubtickMoveStep* (*allocate_subtick_move)(google::protobuf::Arena* arena) = nullptr;

void UserCmd::resolve_signatures()
{
	usercmd_list
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8D 05 ? ? ? ? 4C 89 EF 4C 8B 30 E8">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(usercmd_list), usercmd_list);
	cmdlist_index_from_entity
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 89 C6 31 C0 83 FE FF 0F 95 C0 4C 89 F7 29 C6 E8 ? ? ? ? 4C 89 EF">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(cmdlist_index_from_entity), cmdlist_index_from_entity);
	get_usercmds
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 4C 89 EF 49 89 C6 8B 80">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(get_usercmds), get_usercmds);
	get_usercmd
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 49 89 C7 48 8B 05 ? ? ? ? 66 0F 6E 40">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .BCRL_EXPECT(decltype(get_usercmd), get_usercmd);
	allocate_subtick_move
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"16CSubtickMoveStep">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::absolute(),
				  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
			  .sub(8)
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libclient.so"))
			  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 e5">())
			  .BCRL_EXPECT(decltype(allocate_subtick_move), allocate_subtick_move);
}

UserCmd* UserCmd::get_current_command(BasePlayerController* controller)
{
	// Controller could be a different slot, but eh, who cares...
	const int current_usercmd_index = Interfaces::source2_client->get_command_index(0);
	return RetAddrSpoofer::invoke(get_usercmd, controller, current_usercmd_index);
}

CSubtickMoveStep* UserCmd::allocate_new_move_step(float when, bool completely_empty)
{
	if (!csgo_usercmd.has_base())
		return nullptr;

	auto* subtick_moves = csgo_usercmd.mutable_base()->mutable_subtick_moves();

	CSubtickMoveStep* new_step = nullptr;

	if (!completely_empty && !subtick_moves->empty()) {
		auto it
			= *subtick_moves
			| std::ranges::views::filter([](CSubtickMoveStep& move_step) {
				  return !move_step.has_button();
			  });

		if (!it.empty()) {
			auto closest = std::ranges::min_element(it, {}, [when](CSubtickMoveStep& move_step) { return std::abs(when - move_step.when()); });
			new_step = std::to_address(closest);
		}
	}

	// Didn't find anything yet? Fine okay let's allocate one.
	if (!new_step) {
		new_step = RetAddrSpoofer::invoke(allocate_subtick_move, subtick_moves->GetArena());
		subtick_moves->AddAllocated(new_step);
	}

	new_step->set_when(when);

	std::ranges::stable_sort(*subtick_moves, {}, [](const CSubtickMoveStep& move_step) { return move_step.when(); });

	return new_step;
}

void UserCmd::set_buttonstate1(std::uint64_t value)
{
	if (csgo_usercmd.has_base() && csgo_usercmd.base().has_buttons_pb())
		csgo_usercmd.mutable_base()->mutable_buttons_pb()->set_buttonstate1(value);
	buttons.buttonstate1 = value;
}

void UserCmd::set_buttonstate2(std::uint64_t value)
{
	if (csgo_usercmd.has_base() && csgo_usercmd.base().has_buttons_pb())
		csgo_usercmd.mutable_base()->mutable_buttons_pb()->set_buttonstate2(value);
	buttons.buttonstate2 = value;
}

void UserCmd::set_buttonstate3(std::uint64_t value)
{
	if (csgo_usercmd.has_base() && csgo_usercmd.base().has_buttons_pb())
		csgo_usercmd.mutable_base()->mutable_buttons_pb()->set_buttonstate3(value);
	buttons.buttonstate3 = value;
}

void UserCmd::fixup_buttons_for_move(float last_forwardmove, float last_leftmove, const Buttons& last_buttons)
{
	// TODO The 28th July update made it that the forwardmove and leftmove are only processed when buttons are pressed
	// This caused this function to get dumbed down a bit. I think this is wrong but currently the only way of getting
	// forwardmove and leftmove to be accepted again.

	std::uint64_t buttons = this->buttons.buttonstate1;

	const float forward = csgo_usercmd.base().forwardmove();
	const float left = csgo_usercmd.base().leftmove();

	if (forward > 0.0F) {
		buttons |= IN_FORWARD;
		buttons &= ~IN_BACK;
	} else if (forward < 0.0F) {
		buttons |= IN_BACK;
		buttons &= ~IN_FORWARD;
	} else {
		buttons &= ~IN_FORWARD;
		buttons &= ~IN_BACK;
		csgo_usercmd.mutable_base()->clear_forwardmove();
	}

	if (left > 0.0F) {
		buttons |= IN_MOVELEFT;
		buttons &= ~IN_MOVERIGHT;
	} else if (left < 0.0F) {
		buttons &= ~IN_MOVELEFT;
		buttons |= IN_MOVERIGHT;
	} else {
		buttons &= ~IN_MOVELEFT;
		buttons &= ~IN_MOVERIGHT;
		csgo_usercmd.mutable_base()->clear_leftmove();
	}

	set_buttonstate1(buttons);

	static constexpr std::uint64_t MOVE_MASK = IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;
	const std::uint64_t last_move = last_buttons.buttonstate1 & MOVE_MASK;
	const std::uint64_t next_move = this->buttons.buttonstate1 & MOVE_MASK;
	const std::uint64_t changes = last_move ^ next_move;
	set_buttonstate2((this->buttons.buttonstate2 & ~MOVE_MASK) | changes);

	auto* list = csgo_usercmd.mutable_base()->mutable_subtick_moves();

	while (true) {
		auto it = std::ranges::find_if(*list,
			[](CSubtickMoveStep& move_step) { return move_step.button() & MOVE_MASK; });
		if (it == list->end())
			break;
		list->erase(it);
	}

	// TODO randomize timings

	// The reason why I'm doing this in this odd order is to keep the important buttons at the top of the list of subticks, so that if we exceed
	// the maximum subtick moves, the most important movements are still processed.

	// Let's enable the buttons that are newly pressed
	for (const std::uint64_t bit : { IN_FORWARD, IN_BACK, IN_MOVELEFT, IN_MOVERIGHT }) {
		if (!(this->buttons.buttonstate2 & bit))
			continue; // unchanged
		if (!(buttons & bit))
			continue;
		CSubtickMoveStep* step = allocate_new_move_step(0.005F);
		step->set_pressed(true);
		step->set_button(bit);
	}

	// Now let's disable the buttons that are no longer pressed
	for (const std::uint64_t bit : { IN_FORWARD, IN_BACK, IN_MOVELEFT, IN_MOVERIGHT }) {
		if (!(this->buttons.buttonstate2 & bit))
			continue; // unchanged
		if (!(buttons & bit))
			continue;
		CSubtickMoveStep* step = allocate_new_move_step(0.005F);
		step->set_pressed(false);
		step->set_button(bit);
	}
}

void UserCmd::spread_out_rotation_changes(float old_yaw, float old_pitch)
{
	const float pitch = csgo_usercmd.base().viewangles().x();
	const float yaw = csgo_usercmd.base().viewangles().y();

	const float delta_yaw = yaw - old_yaw;
	const float delta_pitch = pitch - old_pitch;

	const float delta_yaw_per_subtick = delta_yaw / static_cast<float>(csgo_usercmd.base().subtick_moves_size());
	const float delta_pitch_per_subtick = delta_pitch / static_cast<float>(csgo_usercmd.base().subtick_moves_size());

	for (CSubtickMoveStep& move_step : *csgo_usercmd.mutable_base()->mutable_subtick_moves()) {
		move_step.set_analog_yaw_delta(delta_yaw_per_subtick);
		move_step.set_analog_pitch_delta(delta_pitch_per_subtick);
	}
}

// Because protobufs DebugString() has issues beyond my comprehension and I can't be bothered fixing filthy google software, here we go:

#define PRETTY_PRINT(member) pretty_print(s, depth + 1, #member, value.member)
#define PRETTY_PRINT_BITMASK(member) pretty_print_bitmask(s, depth + 1, #member, value.member)
#define PRETTY_PRINT_PROTO_VEC(member) pretty_print(s, depth + 1, #member, value.member())
#define PRETTY_PRINT_PROTO(member) \
	if (value.has_##member())      \
	pretty_print(s, depth + 1, #member, value.member())
#define PRETTY_PRINT_PROTO_BITMASK(member) \
	if (value.has_##member())              \
	pretty_print_bitmask(s, depth + 1, #member, value.member())

static void pretty_print(std::string& s, int depth, std::string_view name, bool value)
{
	s += std::format("{}{}: {}\n", std::string(depth, '\t'), name, value);
};

static void pretty_print(std::string& s, int depth, std::string_view name, int value)
{
	s += std::format("{}{}: {}\n", std::string(depth, '\t'), name, value);
};

static void pretty_print(std::string& s, int depth, std::string_view name, std::uint32_t value)
{
	s += std::format("{}{}: {}\n", std::string(depth, '\t'), name, value);
};

static void pretty_print_bitmask(std::string& s, int depth, std::string_view name, std::uint64_t value)
{
	// TODO perhaps our knowledge of the inputbitmask enum, would allow us to find some names for each bit
	s += std::format("{}{}: {:b}\n", std::string(depth, '\t'), name, value);
};

static void pretty_print(std::string& s, int depth, std::string_view name, float value)
{
	s += std::format("{}{}: {}\n", std::string(depth, '\t'), name, value);
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CMsgQAngle& value)
{
	s += std::format("{}{}: ({};{};{})\n", std::string(depth, '\t'), name, value.x(), value.y(), value.z());
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CMsgVector& value)
{
	s += std::format("{}{}: ({};{};{})\n", std::string(depth, '\t'), name, value.x(), value.y(), value.z());
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CSGOInterpolationInfoPB_CL& value)
{
	if (value.has_frac())
		pretty_print(s, depth, name, value.frac());
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CSGOInterpolationInfoPB& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	PRETTY_PRINT_PROTO(frac);
	PRETTY_PRINT_PROTO(src_tick);
	PRETTY_PRINT_PROTO(dst_tick);

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CSGOInputHistoryEntryPB& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	PRETTY_PRINT_PROTO(view_angles);
	PRETTY_PRINT_PROTO(cl_interp);
	PRETTY_PRINT_PROTO(sv_interp0);
	PRETTY_PRINT_PROTO(sv_interp1);
	PRETTY_PRINT_PROTO(player_interp);
	PRETTY_PRINT_PROTO(shoot_position);
	PRETTY_PRINT_PROTO(target_head_pos_check);
	PRETTY_PRINT_PROTO(target_abs_pos_check);
	PRETTY_PRINT_PROTO(target_abs_ang_check);
	PRETTY_PRINT_PROTO(render_tick_count);
	PRETTY_PRINT_PROTO(render_tick_fraction);
	PRETTY_PRINT_PROTO(player_tick_count);
	PRETTY_PRINT_PROTO(player_tick_fraction);
	PRETTY_PRINT_PROTO(frame_number);
	PRETTY_PRINT_PROTO(target_ent_index);

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

template <typename T>
static void pretty_print(std::string& s, int depth, std::string_view name, const google::protobuf::RepeatedPtrField<T>& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	int i = 0;

	for (const T& input : value) {
		pretty_print(s, depth + 1, std::to_string(i), input);
		i++;
	}

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CSubtickMoveStep& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	PRETTY_PRINT_PROTO_BITMASK(button);
	PRETTY_PRINT_PROTO(pressed);
	PRETTY_PRINT_PROTO(when);
	PRETTY_PRINT_PROTO(analog_forward_delta);
	PRETTY_PRINT_PROTO(analog_left_delta);
	PRETTY_PRINT_PROTO(analog_pitch_delta);
	PRETTY_PRINT_PROTO(analog_yaw_delta);

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CInButtonStatePB& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	PRETTY_PRINT_PROTO_BITMASK(buttonstate1);
	PRETTY_PRINT_PROTO_BITMASK(buttonstate2);
	PRETTY_PRINT_PROTO_BITMASK(buttonstate3);

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

static void pretty_print(std::string& s, int depth, std::string_view name, const Buttons& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	PRETTY_PRINT_BITMASK(buttonstate1);
	PRETTY_PRINT_BITMASK(buttonstate2);
	PRETTY_PRINT_BITMASK(buttonstate3);

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CBaseUserCmdPB& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	PRETTY_PRINT_PROTO_VEC(subtick_moves);
	// PRETTY_PRINT_PROTO(move_crc);
	PRETTY_PRINT_PROTO(buttons_pb);
	PRETTY_PRINT_PROTO(viewangles);
	PRETTY_PRINT_PROTO(legacy_command_number);
	PRETTY_PRINT_PROTO(client_tick);
	PRETTY_PRINT_PROTO(forwardmove);
	PRETTY_PRINT_PROTO(leftmove);
	PRETTY_PRINT_PROTO(upmove);
	PRETTY_PRINT_PROTO(impulse);
	PRETTY_PRINT_PROTO(weaponselect);
	PRETTY_PRINT_PROTO(random_seed);
	PRETTY_PRINT_PROTO(mousedx);
	PRETTY_PRINT_PROTO(mousedy);
	PRETTY_PRINT_PROTO(prediction_offset_ticks_x256);
	PRETTY_PRINT_PROTO(consumed_server_angle_changes);
	PRETTY_PRINT_PROTO(cmd_flags);
	PRETTY_PRINT_PROTO(pawn_entity_handle);

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

static void pretty_print(std::string& s, int depth, std::string_view name, const CSGOUserCmdPB& value)
{
	s += std::format("{}{} {{\n", std::string(depth, '\t'), name);

	PRETTY_PRINT_PROTO_VEC(input_history);
	PRETTY_PRINT_PROTO(base);
	PRETTY_PRINT_PROTO(left_hand_desired);
	PRETTY_PRINT_PROTO(is_predicting_body_shot_fx);
	PRETTY_PRINT_PROTO(is_predicting_head_shot_fx);
	PRETTY_PRINT_PROTO(is_predicting_kill_ragdolls);
	PRETTY_PRINT_PROTO(attack1_start_history_index);
	PRETTY_PRINT_PROTO(attack2_start_history_index);
	PRETTY_PRINT_PROTO(attack3_start_history_index);

	s += std::format("{}}}\n", std::string(depth, '\t'));
};

std::string UserCmd::stringify() const
{
	std::string s;

	const int depth = 0;
	const auto& value = *this;

	s += "usercmd {\n";
	PRETTY_PRINT(csgo_usercmd);
	PRETTY_PRINT(buttons);
	PRETTY_PRINT(has_been_predicted);
	s += "}\n";

	return s;
}
