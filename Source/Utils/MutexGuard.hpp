#pragma once

#include <mutex>
#include <optional>

// Inspired by https://github.com/p-avital/cpp-mutex-guard
// Don't want to include it directly as a dependency, because the Mutex wrapper type is something I don't really need...
// TODO special thanks sections for proper attribution

template <typename T, typename Lock = std::lock_guard<std::mutex>>
class MutexGuard {
	T* value;
	std::optional<Lock> lock;

public:
	explicit MutexGuard(T* value, Lock lock)
		: value(value)
		, lock(std::move(lock))
	{
	}

	// TODO remove pointer on mutex
	explicit MutexGuard(T* value, Lock::mutex_type& mutex)
		: value(value)
		, lock(mutex)
	{
	}

	~MutexGuard() = default;

	T& operator*() const noexcept { return *value; }
	T* operator->() const noexcept { return value; }

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	operator T*() noexcept
	{
		return value;
	}

	T* get(this auto&& self) noexcept
	{
		return self.value;
	}

	MutexGuard(const MutexGuard&) = delete;
	MutexGuard& operator=(const MutexGuard&) = delete;

	MutexGuard(MutexGuard&& other) noexcept
		: value(other.value)
		, lock(other.lock)
	{
		other.value = nullptr;
		other.lock.reset();
	}

	MutexGuard& operator=(MutexGuard&& guard) noexcept
	{
		value = guard.value;
		lock = guard.lock;
		guard.lock.reset();
	}
};
