#include "SchemaClassInfo.hpp"

#include "Types/ParentData.hpp"

bool SchemaClassInfo::bases_on(SchemaClassInfo* other) const
{
	const SchemaClassInfo* curr = this;

	while(curr) {
		if(curr == other) return true;

		ParentData* parent = curr->parent;
		if(!parent) break;
		curr = parent->class_info;
	}

	return false;
}
