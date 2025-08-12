#include "GameEntitySystem.hpp"

#include "../EntityHandle.hpp"
#include "BaseEntity.hpp"
#include "EntityIdentity.hpp"
#include "EntityInstance.hpp"

#include "RetAddrSpoofer.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "../../Hooks/Hooks.hpp"
#include "../../Memory.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include <cstddef>
#include <cstdint>
#include <shared_mutex>

static GameEntitySystem** game_entity_system = nullptr;

static UninitializedObject<Hooks::DetourHook<true>> on_add_entity_hook;
static UninitializedObject<Hooks::DetourHook<true>> on_remove_entity_hook;

// TODO at some point I would like to have this build up a local copy of the entity list
static void hk_on_add_entity(GameEntitySystem* thisptr, EntityInstance* entity, EntityHandle<EntityInstance> handle)
{
	const auto lock = GameEntitySystem::unique_lock();
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(on_add_entity_hook->get_trampoline()), thisptr, entity, handle);
}

static void hk_on_remove_entity(GameEntitySystem* thisptr, EntityInstance* entity, EntityHandle<EntityInstance> handle)
{
	const auto lock = GameEntitySystem::unique_lock();
	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(on_remove_entity_hook->get_trampoline()), thisptr, entity, handle);
}

std::shared_mutex GameEntitySystem::entity_list_lock{};

void GameEntitySystem::resolve_signatures()
{
	game_entity_system
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"client_entities">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			  .find_xrefs(
				  SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 89 1d">())
			  .add(3)
			  .relative_to_absolute()
			  .BCRL_EXPECT(GameEntitySystem**, game_entity_system);

	const auto get_vtable_function = [](std::size_t vtable_index) {
		return BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"17CGameEntitySystem">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			.find_xrefs(SignatureScanner::XRefTypes::absolute(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			.sub(sizeof(void*))
			.find_xrefs(SignatureScanner::XRefTypes::absolute(),
				BCRL::everything(Memory::mem_mgr).with_flags("r--").with_name("libclient.so"))
			.add(sizeof(void*))
			.add(vtable_index * sizeof(void*))
			.dereference();
	};

	void* on_add_entity
		= get_vtable_function(GameEntitySystem::on_add_entity_index)
			  .BCRL_EXPECT(void*, on_add_entity);
	void* on_remove_entity
		= get_vtable_function(GameEntitySystem::on_remove_entity_index)
			  .BCRL_EXPECT(void*, on_remove_entity);

	on_add_entity_hook.emplace(
		Memory::emalloc,
		on_add_entity,
		reinterpret_cast<void*>(hk_on_add_entity));

	on_remove_entity_hook.emplace(
		Memory::emalloc,
		on_remove_entity,
		reinterpret_cast<void*>(hk_on_remove_entity));

	on_add_entity_hook->enable();
	on_remove_entity_hook->enable();
}

GameEntitySystem* GameEntitySystem::the()
{
	return *game_entity_system;
}

BaseEntity* GameEntitySystem::get_entity_by_index(unsigned int index)
{
	if (index >= GameEntitySystem::ENTITY_LIST_INDEX_MASK)
		return nullptr;

	static constexpr int HASH_KEY_SHIFT = 9;
	EntityIdentity* bucket = entity_buckets()[index >> HASH_KEY_SHIFT];

	if (!bucket)
		return nullptr;

	static const std::uint32_t SIZEOF_ENTITYIDENTITY = EntityIdentity::classInfo()->size;

	static constexpr int BUCKET_INDEX_MASK = (1 << HASH_KEY_SHIFT) - 1;
	const std::size_t bucket_index = index & BUCKET_INDEX_MASK;

	// Since our EntityIdentity has a different size than the real one, we need to do some indexing magic here...
	const EntityIdentity& entity_identity = *reinterpret_cast<EntityIdentity*>(reinterpret_cast<std::byte*>(bucket) + bucket_index * SIZEOF_ENTITYIDENTITY);

	if ((entity_identity.entry_index & GameEntitySystem::ENTITY_LIST_INDEX_MASK) != index)
		return nullptr;

	return static_cast<BaseEntity*>(entity_identity.instance);
}
