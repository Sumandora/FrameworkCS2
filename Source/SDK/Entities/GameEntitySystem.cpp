#include "GameEntitySystem.hpp"

#include "../../Memory.hpp"
#include "../../Utils/Logging.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

int (*GameEntitySystem::get_highest_entity_index_ptr)(GameEntitySystem* thisptr) = nullptr;
BaseEntity* (*GameEntitySystem::get_base_entity_ptr)(GameEntitySystem* thisptr, int index) = nullptr;

GameEntitySystem** GameEntitySystem::find()
{
	auto** game_entity_system
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8D 05 ? ? ? ? 55 83 C6 01">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .expect<GameEntitySystem**>("Couldn't find GameEntitySystem");
	get_base_entity_ptr
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"31 C0 81 FE FE 7F 00 00 77 ? 89 F0">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .expect<decltype(get_base_entity_ptr)>("Couldn't find getBaseEntity");
	Logging::info("Found getBaseEntity at: {}", get_base_entity_ptr);

	return game_entity_system;
}

GameEntitySystem* GameEntitySystem::the()
{
	static GameEntitySystem** game_entity_system = find();
	return *game_entity_system;
}
