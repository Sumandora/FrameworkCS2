#pragma once

#include <chrono>

template <typename Duration, auto N>
class Interval {
	std::chrono::system_clock::time_point last_update;

public:
	// @param start_off_as_passed indicates if the first call to `passed()` should always yield passed true
	explicit Interval(bool start_off_as_passed = true)
		: last_update(start_off_as_passed
				  ? std::chrono::system_clock::time_point{}
				  : std::chrono::system_clock::now())
	{
	}

	bool passed(bool auto_reset = true)
	{
		bool passed = std::chrono::system_clock::now() - last_update > Duration(N);
		if (auto_reset && passed)
			reset();
		return passed;
	}

	void reset()
	{
		last_update = std::chrono::system_clock::now();
	}
};
