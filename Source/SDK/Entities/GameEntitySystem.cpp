#include "GameEntitySystem.hpp"

#include "../../Memory.hpp"

#include "BaseEntity.hpp"
#include "EntityIdentity.hpp"
#include "EntityInstance.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include <cstddef>
#include <cstdint>

static GameEntitySystem** game_entity_system = nullptr;

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
