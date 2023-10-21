#pragma once

template <typename T>
class CUtlMemory {
public:
	T* memory;
	int allocationCount;
	int growSize;
};
