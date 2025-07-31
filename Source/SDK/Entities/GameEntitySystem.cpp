#include "GameEntitySystem.hpp"

#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "RetAddrSpoofer.hpp"

#include "BaseEntity.hpp"

static GameEntitySystem** game_entity_system = nullptr;
static BaseEntity* (*get_entity_by_index)(GameEntitySystem* thisptr, int index) = nullptr;

void GameEntitySystem::resolve_signatures()
{
	game_entity_system
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"48 8D 05 ? ? ? ? 55 83 C6 01">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .add(3)
			  .relative_to_absolute()
			  .BCRL_EXPECT(GameEntitySystem**, game_entity_system);

	::get_entity_by_index
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"31 C0 81 FE FE 7F 00 00 77 ? 89 F0">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .BCRL_EXPECT(decltype(::get_entity_by_index), ::get_entity_by_index);
}

GameEntitySystem* GameEntitySystem::the()
{
	return *game_entity_system;
}

BaseEntity* GameEntitySystem::get_entity_by_index(int index)
{
	// TODO rebuild
	return RetAddrSpoofer::invoke(::get_entity_by_index, this, index);
}
