#pragma once

#include "../../Memory.hpp"

struct BaseEntity;

class GameEntitySystem {
public:
	GameEntitySystem();
	[[nodiscard]] int getHighestEntityIndex() const;
	[[nodiscard]] BaseEntity* getBaseEntity(int index) const;

private:
	void** thisPtrPtr;
	void* getHighestEntityIndexPtr;
	void* getBaseEntityPtr;
};