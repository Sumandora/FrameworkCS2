#pragma once

#include <cstddef>
#include <utility>

template <typename T>
class UninitializedObject {
	alignas(T) std::byte storage[sizeof(T)];

public:
	using Type = T;

	template <typename... Args>
	T* emplace(Args&&... args)
	{
		T* inner = new (&storage) T{ std::forward<Args>(args)... };
		return inner;
	}

	T* get(this auto&& self) noexcept
	{
		return reinterpret_cast<T*>(&self.storage);
	}

	T* operator->(this auto&& self) noexcept
	{
		return self.get();
	}

	void reset()
	{
		get()->~T();
	}

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	operator T&() { return *get(); }
};
