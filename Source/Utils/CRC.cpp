#include "CRC.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "Logging.hpp"

#include "RetAddrSpoofer.hpp"

#include "../Memory.hpp"

#include "../SDK/CUtl/Buffer.hpp"
#include "../SDK/GameClass/MemAlloc.hpp"

#include "google/protobuf/arena.h"

#include <cstddef>
#include <cstring>
#include <format>
#include <vector>

// The game accesses move_crc in an unusual way, I suspect that either write_message or set_message_data is actually a protobuf function like MergeFrom.

// NOLINTNEXTLINE
#define private public
#include "usercmd.pb.h"
#undef private

#include "../SDK/GameClass/UserCmd.hpp"

static int (*create_new_base_cmd)(CBaseUserCmdPB* cmd, int, int) = nullptr;
static bool (*set_message_data)(void* move_crc, char* crc_result, google::protobuf::Arena* arena) = nullptr;

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
	set_message_data
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"41 56 41 55 41 54 55 48 89 FD 53 48 83 EC 10 F6 07 03">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .expect<decltype(set_message_data)>("Couldn't find set_message_data");
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
	const std::size_t crc_size = new_base_cmd.ByteSizeLong();
	buffer.ensure_capacity(static_cast<int>(crc_size + 1));

	const bool success = new_base_cmd.SerializePartialToArray(buffer.memory.memory, static_cast<int>(crc_size));
	if (!success) {
		Logging::error("Failed to generate CRC");
		return false;
	}

	struct GameString {
		std::size_t size;
		std::size_t size2;
		void* pad2;
		char data[1]; // VLA
	};

	auto* puVar20 = static_cast<GameString*>(MemAlloc::the()->allocate(crc_size + 0x19));
	puVar20->size = crc_size;
	puVar20->size2 = crc_size;
	void* puVar26 = &puVar20->data;
	puVar20->pad2 = nullptr;
	puVar26 = std::memcpy(puVar26, buffer.memory.memory, crc_size);

	void* crc = &usercmd->csgo_usercmd.mutable_base()->_impl_.move_crc_;
	usercmd->csgo_usercmd.mutable_base()->_impl_._has_bits_.has_bits_[0] |= 1;

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(set_message_data), crc, static_cast<void*>(&puVar26), usercmd->csgo_usercmd.base().GetArena());

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
