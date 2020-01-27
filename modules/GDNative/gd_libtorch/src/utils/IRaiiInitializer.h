#pragma once

namespace smidi
{
	/**
	 * IRaiiInitializer
	 * 
	 * base class for hideinng implementation details
	 * in implementations of derrived classes
	 */
	class IRaiiInitializer
	{
	public:
		virtual ~IRaiiInitializer() = default;
	};
}
