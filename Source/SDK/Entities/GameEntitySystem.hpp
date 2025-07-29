#pragma once

#include "../../Memory.hpp"

#include "RetAddrSpoofer.hpp"

#include "BaseEntity.hpp"

class GameEntitySystem {
	static int (*get_highest_entity_index_ptr)(GameEntitySystem* thisptr);
	static BaseEntity* (*get_base_entity_ptr)(GameEntitySystem* thisptr, int index);

	static GameEntitySystem** find();

	PADDING(0x2118);
	int highest_entity_index;

public:
	static GameEntitySystem* the();

	int getHighestEntityIndex()
	{
		return 32768; // TODO ghetto
	}

	BaseEntity* getBaseEntity(int index)
	{
		return RetAddrSpoofer::invoke(get_base_entity_ptr, this, index);
	}

	template<typename Ent>
	Ent* find_first_entity_of_type() {
		const int highest = getHighestEntityIndex();
		for (int i = 0; i < highest; i++) {
			BaseEntity* base_entity = getBaseEntity(i);
			if (!base_entity)
				continue;

			if (base_entity->getSchemaType() != Ent::classInfo())
				continue;

			return static_cast<Ent*>(base_entity);
		}
		return nullptr;
	}
};
