#pragma once

#include <cstddef>
#include <utility>

template <typename T>
class UninitializedObject {
	alignas(T) std::byte storage[sizeof(T)];

public:
	template<typename... Args>
	T* emplace(Args... args) {
		T* inner = new (&storage) T{std::forward<Args>(args)...};
		return inner;
	}

	T* get(this auto&& self) {
		return reinterpret_cast<T*>(&self.storage);
	}

	T* operator->(this auto&& self) {
		return self.get();
	}

	void reset() {
		get()->~T();
	}
};
