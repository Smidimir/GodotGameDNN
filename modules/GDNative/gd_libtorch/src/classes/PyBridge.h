#pragma once

#include <utils/IRaiiInitializer.h>

#include <core/Godot.hpp>
#include <gen/Node.hpp>

#include <memory>
#include <vector>

namespace smidi
{
	class PyBridge
		: public godot::Node
	{
		GODOT_CLASS(PyBridge, Node);
	public:
		PyBridge();
		~PyBridge();

		static void _register_methods();

		void _init();

	public:
		godot::Variant get_random_action();
		int get_random_action_cpp();
		godot::Variant get_dnn_action(godot::Array state);
		int get_dnn_action_cpp(std::vector<int> const& state);

		void remember( godot::Array state
		             , godot::Variant action
		             , godot::Variant reward
		             , godot::Array next_state
		             , godot::Variant done
		             );
		void remember_cpp( std::vector<int> const& state
		                 , int action
		                 , int reward
		                 , std::vector<int> const& next_state
		                 , bool done
		                 );
		void train_long_memory();
		void train_short_memory( godot::Array state
		                       , godot::Variant action
		                       , godot::Variant reward
		                       , godot::Array next_state
		                       , godot::Variant done
		                       );
		void train_short_memory_cpp( std::vector<int> const& state
		                           , int action
		                           , int reward
		                           , std::vector<int> const& next_state
		                           , bool done
		                           );

	private:
		std::unique_ptr<IRaiiInitializer> m_pythonEnvInitializer;
	};
}