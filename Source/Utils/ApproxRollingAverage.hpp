#pragma once

#include <cstddef>

// https://stackoverflow.com/a/16757630
template <std::size_t N, typename T>
class ApproxRollingAverage {
	T avg{ 0 };

public:
	T push(T new_sample)
	{
		avg -= avg / N;
		avg += new_sample / N;

		return avg;
	}

	T get_average() const
	{
		return avg;
	}
};
