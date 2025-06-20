#include <cstddef>

#include "CRC.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "Logging.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "../Memory.hpp"

#include "../SDK/CUtl/Buffer.hpp"
#include "../SDK/GameClass/MemAlloc.hpp"

// The game accesses move_crc in an unusual way, I suspect that either write_message or set_message_data is actually a protobuf function like MergeFrom.

// NOLINTNEXTLINE
#define private public
#include "usercmd.pb.h"
#undef private

#include "../SDK/GameClass/UserCmd.hpp"

void CRC::update_crc(UserCmd* usercmd)
{
	static auto* create_new_base_cmd
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 48 8B 45 ? 83 8D ? ? ? ? 04">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<int (*)(CBaseUserCmdPB* cmd, int, int)>("Couldn't find create_new_base_cmd");
	static auto* calculate_crc_size
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 4C 89 FF 8D 70 ? 48 89 C3">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<int (*)(CBaseUserCmdPB* cmd)>("Couldn't find calculate_crc_size");
	static auto* serialize
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"41 56 41 55 41 89 D6 41 54 55 49 89 FC">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .expect<bool (*)(CBaseUserCmdPB* cmd, void* buf, int size)>("Couldn't find serialize");
	static auto* write_message // This is what windows folks call this, I think the name is misleading... it's more like `create_string` or something like that...
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 48 8B 1D ? ? ? ? 48 8B 95">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<char* (*)(void* begin, void* end)>("Couldn't find write_message");
	static auto* set_message_data
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"41 56 41 55 41 54 55 48 89 FD 53 48 83 EC 10 F6 07 03">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .expect<bool (*)(void** move_crc, char** crc_result, void* has_bits)>("Couldn't find set_message_data");
	static auto* empty_str
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8B 1D ? ? ? ? 48 8D 43 ? E9 ? ? ? ? 49 8B 46">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .expect<char**>("Couldn't find empty_str");
	static auto* operator_delete
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? E9 ? ? ? ? ? ? ? ? ? ? ? 49 8B 56 ? 41 83 4E ? 01">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<void* (*)(void*, std::size_t)>("Couldn't find empty_str");

	CBaseUserCmdPB new_base_cmd;
	create_new_base_cmd(&new_base_cmd, 0, 0);

	*new_base_cmd.mutable_viewangles() = usercmd->csgo_usercmd.base().viewangles();

	new_base_cmd.mutable_buttons_pb()->set_buttonstate1(usercmd->buttons.buttonstate1);
	new_base_cmd.mutable_buttons_pb()->set_buttonstate2(usercmd->buttons.buttonstate2);
	new_base_cmd.mutable_buttons_pb()->set_buttonstate3(usercmd->buttons.buttonstate3);

	UtlBuffer buffer(0, 0, 0);
	const int crc_size = calculate_crc_size(&new_base_cmd);
	buffer.ensure_capacity(crc_size + 1);
	const bool success = serialize(&new_base_cmd, buffer.memory.memory, crc_size);
	if (!success) {
		Logging::error("Failed to generate CRC");
		return;
	}

	bool empty = false;
	char* local_110 = nullptr;
	if (crc_size != 0) {
		local_110 = write_message(buffer.memory.memory, buffer.memory.memory + crc_size);
	} else {
		empty = true;
		local_110 = *empty_str + 0x18;
	}

	void* crc = usercmd->csgo_usercmd.mutable_base()->_impl_.move_crc_.UnsafeMutablePointer();
	usercmd->csgo_usercmd.mutable_base()->_impl_._has_bits_.has_bits_[0] |= 1;

	// Previous CRC does not need to be deallocated because it was allocated in the same arena.
	set_message_data(&crc, &local_110, usercmd->csgo_usercmd.base().GetArena());

	if (!empty) {
		// Using operator.delete from the game, because I'm not sure about how the whole string ABI stuff works out here...
		int* var2 = reinterpret_cast<int*>(local_110 + -8);
		const int var28 = *var2;
		*var2 = *var2 + -1;
		if (var28 < 1) {
			operator_delete(local_110 + -0x18, *reinterpret_cast<std::size_t*>(local_110 + -0x10) + 0x19);
		}
	}

	if ((buffer.memory.growSize & 0xc0000000) == 0 && buffer.memory.memory != nullptr)
		MemAlloc::the()->deallocate(buffer.memory.memory);
}
