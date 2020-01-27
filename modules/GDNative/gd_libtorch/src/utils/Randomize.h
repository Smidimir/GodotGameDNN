#pragma once

#include <random>

namespace smidi
{
	std::mt19937_64& get_random_generator();

	template<typename T>
	auto randomize_int(T from, T to)
	{
		std::uniform_int_distribution<T> dist{ from, to };
		return dist(get_random_generator());
	}
}
