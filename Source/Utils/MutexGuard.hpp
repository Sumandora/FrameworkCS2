#pragma once

#include <mutex>

// Inspired by https://github.com/p-avital/cpp-mutex-guard
// Don't want to include it directly as a dependency, because the Mutex wrapper type is something I don't really need...
// TODO special thanks sections for proper attribution

template <typename T>
class MutexGuard {
	T* value;
	std::mutex* mutex;

public:
	explicit MutexGuard(T* value, std::mutex* mutex)
		: value(value)
		, mutex(mutex)
	{
	}

	~MutexGuard()
	{
		mutex->unlock();
	}

	T& operator*() const noexcept { return *value; }
	T* operator->() const noexcept { return value; }

	MutexGuard(const MutexGuard&) = delete;
	MutexGuard& operator=(const MutexGuard&) = delete;

	MutexGuard(MutexGuard&& other) noexcept
		: value(other.value)
		, mutex(other.mutex)
	{
		other.value = nullptr;
		other.mutex = nullptr;
	}

	MutexGuard& operator=(MutexGuard&& guard) noexcept
	{
		value = guard.value;
		mutex = guard.mutex;
		guard.mutex = nullptr;
	}
};
