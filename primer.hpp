#ifndef __PRIMER_HPP__
#define __PRIMER_HPP__

#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <future>
#include <assert.h>

class Primer
{
public:
	// Check if number is prime
	// - Start multiple asynchronous calculations
	// - Note: Requirement is that _all_ calculations be done in parallel
	//		: std::async is effective for large primes, but very heavy weight for small primes
	//		: Solution a) is to use a sync calculation for smaller primes
	//		: Solution b) is to create a threadpool at startup, and manually reuse them (preferred).
	//
	static bool Check(const uint64_t suspect)
	{
		bool prime = true;
		std::vector<std::future<bool>> futures;

		const auto num_slices = degree;
		for (auto i = 0; i < num_slices; ++i)
			futures.push_back(std::async(std::launch::async, CheckSlice, suspect, i, num_slices));

		for (auto &f : futures)
			prime = f.get() && prime;

		return prime;
	}

	// Check primality for a subset of factors
	static bool CheckSlice(const uint64_t suspect, const int slice, const int num_slices)
	{
		assert (slice < num_slices);

		// Handle the edge cases required by the core algorithm
		if (suspect <= 1)
			return false;
		else if (suspect == 2 || suspect == 3)
			return true;
		else if (suspect % 2 == 0 || suspect % 3 == 0)
			return false;

		// Core algorithm
		// - checks in blocks of 6, which only requires 2 of the 6 factors to be tested
		// - stops searching when sqrt(#) is reached.  max_factor is required for overflow protection.
		// - supports parallel calculation by interleaving blocks of 6
		const uint64_t skip = 6 * num_slices;
		const uint64_t start = (slice * 6) + 5;
		for (uint64_t i = start; i <= max_factor && i*i <= suspect; i += skip)
		{
			if (suspect % i == 0 || suspect % (i + 2) == 0)
				return false;
		}
		return true;
	}

private:
	// Maximum number of parallel calculations for this request
	static const auto degree = 4;

	// Upper limit for factor checking
	static const uint64_t max_factor = UINT32_MAX;
};

#endif
