#include "UserCmd.hpp"

#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"

#include <cstdint>

UserCmd* UserCmd::get_current_command(BasePlayerController* controller)
{
	static auto* usercmd_list
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8D 05 ? ? ? ? 4C 89 C7 4C 89 85 ? ? ? ? 4C 8B 28">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .expect<void**>("Couldn't find UserCmd list");
	static auto* cmdlist_index_from_entity
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 89 C6 31 C0 83 FE FF 0F 95 C0 4C 89 EF 29 C6 E8 ? ? ? ? 4C 8B 85">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<int (*)(void*)>("Couldn't find cmdlist_index_from_entity");
	static auto* get_usercmds
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 4C 8B 85 ? ? ? ? 44 8B B8">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<void* (*)(void*, uint64_t)>("Couldn't find get_usercmds");
	static auto* get_usercmd
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"E8 ? ? ? ? 4C 8B 85 ? ? ? ? 49 89 C6 48 8D 05 ? ? ? ? 4D 85 F6">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().with_name("libclient.so"))
			  .add(1)
			  .relative_to_absolute()
			  .expect<UserCmd* (*)(void*, int)>("Couldn't find get_usercmd");

	if (usercmd_list == nullptr) {
		return nullptr;
	}

	int list_index = cmdlist_index_from_entity(controller);

	if (list_index == -1)
		return nullptr;

	list_index -= 1;

	void* usercmds = get_usercmds(*usercmd_list, list_index);

	const int current_usercmd_index = *reinterpret_cast<int*>(reinterpret_cast<char*>(usercmds) + 0x5c00);

	return get_usercmd(controller, current_usercmd_index);
}
