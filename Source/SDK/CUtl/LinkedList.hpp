#pragma once

#include "Memory.hpp"

template<typename T, class I = unsigned short>
struct CUtlLinkedList {
	static constexpr I INVALID_INDEX = (I)~0;
	struct ListElement {
		T element;
		I prev;
		I next;
	};

	CUtlMemory<ListElement> memory;
	I head;
	I tail;
	I firstFree;
	I elementCount; // used
	I totalElements; // allocated
};