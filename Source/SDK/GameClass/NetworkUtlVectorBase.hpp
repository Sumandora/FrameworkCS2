#pragma once

#include <cstdint>

template <typename T>
class NetworkUtlVectorBase {
public:
	std::uint32_t size;
	T* elements;

	template <typename O>
	NetworkUtlVectorBase<O> as()
	{
		return NetworkUtlVectorBase<O>{ size, reinterpret_cast<O*>(elements) };
	}
};
