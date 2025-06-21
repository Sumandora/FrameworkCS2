#include <cstddef>
#include <format>
#include <vector>

#include "CRC.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "Logging.hpp"
#include "RetAddrSpoofer.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "../Memory.hpp"

#include "../SDK/CUtl/Buffer.hpp"
#include "../SDK/GameClass/MemAlloc.hpp"

#include "google/protobuf/arena.h"

// The game accesses move_crc in an unusual way, I suspect that either write_message or set_message_data is actually a protobuf function like MergeFrom.

// NOLINTNEXTLINE
#define private public
#include "usercmd.pb.h"
#undef private

#include "../SDK/GameClass/UserCmd.hpp"

static int (*create_new_base_cmd)(CBaseUserCmdPB* cmd, int, int);
static int (*calculate_crc_size)(CBaseUserCmdPB* cmd);
static bool (*serialize)(CBaseUserCmdPB* cmd, void* buf, int size);
static char* (*write_message)(void* begin, void* end); // This is what windows folks call this, I think the name is misleading... it's more like `create_string` or something like that...
static bool (*set_message_data)(void* move_crc, char** crc_result, google::protobuf::Arena* arena);
static char** empty_str;
static void* (*operator_delete)(void*, std::size_t);

void CRC::resolve_signatures()
{
	create_new_base_cmd
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 48 8B 45 ? 83 8D ? ? ? ? 04">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(create_new_base_cmd)>("Couldn't find create_new_base_cmd");
	calculate_crc_size
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 4C 89 FF 8D 70 ? 48 89 C3">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(calculate_crc_size)>("Couldn't find calculate_crc_size");
	serialize
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"41 56 41 55 41 89 D6 41 54 55 49 89 FC">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .expect<decltype(serialize)>("Couldn't find serialize");
	write_message
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 48 8B 1D ? ? ? ? 48 8B 95">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(write_message)>("Couldn't find write_message");
	set_message_data
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"41 56 41 55 41 54 55 48 89 FD 53 48 83 EC 10 F6 07 03">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .expect<decltype(set_message_data)>("Couldn't find set_message_data");
	empty_str
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8B 1D ? ? ? ? 48 8D 43 ? E9 ? ? ? ? 49 8B 46">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .expect<decltype(empty_str)>("Couldn't find empty_str");
	operator_delete
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? E9 ? ? ? ? ? ? ? ? ? ? ? 49 8B 56 ? 41 83 4E ? 01">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<decltype(operator_delete)>("Couldn't find operator_delete");
}

bool CRC::update_crc(UserCmd* usercmd)
{
	CBaseUserCmdPB new_base_cmd;
	RetAddrSpoofer::invoke(create_new_base_cmd, &new_base_cmd, 0, 0);

	// NOTE: Don't use the copy constructor, it will use MergeFrom, which will omit zeroed elements, however the game expects those to be there.
	const CMsgQAngle& from = usercmd->csgo_usercmd.base().viewangles();
	CMsgQAngle* to = new_base_cmd.mutable_viewangles();

	to->set_x(from.x());
	to->set_y(from.y());
	to->set_z(from.z());

	CInButtonStatePB* buttons = new_base_cmd.mutable_buttons_pb();

	buttons->set_buttonstate1(usercmd->buttons.buttonstate1);
	buttons->set_buttonstate2(usercmd->buttons.buttonstate2);
	buttons->set_buttonstate3(usercmd->buttons.buttonstate3);

	UtlBuffer buffer(0, 0, 0);
	const int crc_size = RetAddrSpoofer::invoke(calculate_crc_size, &new_base_cmd);
	buffer.ensure_capacity(crc_size + 1);
	const bool success = RetAddrSpoofer::invoke(serialize, &new_base_cmd, static_cast<void*>(buffer.memory.memory), crc_size);
	if (!success) {
		Logging::error("Failed to generate CRC");
		return false;
	}

	bool empty = false;
	char* local_110 = nullptr;
	if (crc_size != 0) {
		local_110 = RetAddrSpoofer::invoke(write_message, static_cast<void*>(buffer.memory.memory), static_cast<void*>(buffer.memory.memory + crc_size));
	} else {
		empty = true;
		local_110 = *empty_str + 0x18;
	}

	void* crc = &usercmd->csgo_usercmd.mutable_base()->_impl_.move_crc_;
	usercmd->csgo_usercmd.mutable_base()->_impl_._has_bits_.has_bits_[0] |= 1;

	// Previous CRC does not need to be deallocated because it was allocated in the same arena.
	RetAddrSpoofer::invoke(set_message_data, crc, &local_110, usercmd->csgo_usercmd.base().GetArena());

	if (!empty) {
		// Using operator.delete from the game, because I'm not sure about how the whole string ABI stuff works out here...
		int* var2 = reinterpret_cast<int*>(local_110 + -8);
		const int var28 = *var2;
		*var2 = *var2 + -1;
		if (var28 < 1) {
			RetAddrSpoofer::invoke(operator_delete, static_cast<void*>(local_110 + -0x18), *reinterpret_cast<std::size_t*>(local_110 + -0x10) + 0x19);
		}
	}

	if ((buffer.memory.growSize & 0xc0000000) == 0 && buffer.memory.memory != nullptr)
		MemAlloc::the()->deallocate(buffer.memory.memory);

	return true;
}

std::vector<char> CRC::extract_crc(UserCmd* usercmd)
{
	if (!usercmd->csgo_usercmd.has_base() || !usercmd->csgo_usercmd.base().has_move_crc())
		return {};

	const void* thisptr = usercmd->csgo_usercmd.mutable_base()->_impl_.move_crc_.UnsafeGetPointer();
	const char* str = *static_cast<const char* const*>(thisptr);
	const std::size_t length = *reinterpret_cast<const std::size_t*>(str - 0x18);
	return { str, str + length };
}
