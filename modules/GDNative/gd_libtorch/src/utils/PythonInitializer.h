#pragma once

#include <utils/IRaiiInitializer.h>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace smidi
{
	class CPythonInitializer : public IRaiiInitializer
	{
	public:
		CPythonInitializer();
		~CPythonInitializer();

		int get_dnn_action(std::vector<int> const& state);
		void remember( std::vector<int> const& state
		             , int action
		             , int reward
		             , std::vector<int> const& next_state
		             , bool done
		             );
		void train_long_memory();
		void train_short_memory( std::vector<int> const& state
		                       , int action
		                       , int reward
		                       , std::vector<int> const& next_state
		                       , bool done
		                       );

	private:
		pybind11::scoped_interpreter m_python_scope{};
		pybind11::module m_module_python_env{};
		pybind11::module m_module_agent{};
		pybind11::object m_agent{};
	};
}
