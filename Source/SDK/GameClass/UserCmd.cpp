#include "UserCmd.hpp"

#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include <cstdint>

static void** usercmd_list = nullptr;
static int (*cmdlist_index_from_entity)(BasePlayerController*) = nullptr;
static void* (*get_usercmds)(void*, uint64_t) = nullptr;
static UserCmd* (*get_usercmd)(BasePlayerController*, int) = nullptr;

void UserCmd::resolve_signatures()
{
	usercmd_list
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8D 05 ? ? ? ? 4C 89 C7 4C 89 85 ? ? ? ? 4C 8B 28">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .expect<decltype(usercmd_list)>("Couldn't find UserCmd list");
	cmdlist_index_from_entity
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 89 C6 31 C0 83 FE FF 0F 95 C0 4C 89 EF 29 C6 E8 ? ? ? ? 4C 8B 85">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(cmdlist_index_from_entity)>("Couldn't find cmdlist_index_from_entity");
	get_usercmds
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 4C 8B 85 ? ? ? ? 44 8B B8">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(get_usercmds)>("Couldn't find get_usercmds");
	get_usercmd
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 4C 8B 85 ? ? ? ? 49 89 C6 48 8D 05 ? ? ? ? 4D 85 F6">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(get_usercmd)>("Couldn't find get_usercmd");
}

UserCmd* UserCmd::get_current_command(BasePlayerController* controller)
{
	int list_index = RetAddrSpoofer::invoke(cmdlist_index_from_entity, controller);

	if (list_index == -1)
		return nullptr;

	list_index -= 1;

	void* usercmds = RetAddrSpoofer::invoke(get_usercmds, *usercmd_list, static_cast<std::uint64_t>(list_index));

	const int current_usercmd_index = *reinterpret_cast<int*>(reinterpret_cast<char*>(usercmds) + 0x5c00);

	return RetAddrSpoofer::invoke(get_usercmd, controller, current_usercmd_index);
}

CSubtickMoveStep* UserCmd::allocate_new_move_step(float when)
{
	static auto* allocate_subtick_move
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
			  .expect<CSubtickMoveStep* (*)(google::protobuf::Arena * arena)>("Couldn't find allocate subtick move");

	if (!csgo_usercmd.has_base())
		return nullptr;

	auto* subtick_moves = csgo_usercmd.mutable_base()->mutable_subtick_moves();

	CSubtickMoveStep* new_step = RetAddrSpoofer::invoke(allocate_subtick_move, subtick_moves->GetArena());

	new_step->set_when(when);

	// TODO: Reorder move steps when the 'when' lies in between other move steps, e.g. when whens look like this 0.1, 0.2, 0.15, they should be reordered to 0.1, 0.15, 0.2

	subtick_moves->AddAllocated(new_step);

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

void UserCmd::fixup_buttons_for_move()
{
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
	}

	if (left > 0.0F) {
		buttons |= IN_MOVERIGHT;
		buttons &= ~IN_MOVELEFT;
	} else if (left < 0.0F) {
		buttons |= IN_MOVELEFT;
		buttons &= ~IN_MOVERIGHT;
	} else {
		buttons &= ~IN_MOVELEFT;
		buttons &= ~IN_MOVERIGHT;
	}

	set_buttonstate1(buttons);
}

void UserCmd::fixup_button_changes(const Buttons& old_buttons)
{
	set_buttonstate2(buttons.buttonstate1 ^ old_buttons.buttonstate1);
}
