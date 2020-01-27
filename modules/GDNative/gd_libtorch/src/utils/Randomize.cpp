#include <pch.h>

#include "Randomize.h"

namespace smidi
{
	std::mt19937_64& get_random_generator()
	{
		static auto gen = std::mt19937_64{ std::random_device{}() };

		return gen;
	}
}
