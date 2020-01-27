#include <pch.h>

#include "PyBridge.h"

#include <utils/PythonInitializer.h>
#include <utils/Randomize.h>

#include <core/Array.hpp>

#include <vector>

namespace smidi
{
	template <typename T>
	auto godot_array_to_vector(godot::Array& arr)
	{
		std::vector<T> vec;
		vec.reserve(arr.size());

		for (std::size_t i = 0; i < arr.size(); ++i)
			vec.push_back(static_cast<T>(arr[i]));

		return vec;
	}

	enum class NEXT_MOVE_DIRECTION : int
	{
		FORWARD = 0,
		LEFT    = 1,
		RIGHT   = 2
	};

	PyBridge::PyBridge()
		: m_pythonEnvInitializer{ std::make_unique<CPythonInitializer>() }
	{
		std::printf("PyBridge::PyBridge\n");
	}

	PyBridge::~PyBridge()
	{
		std::printf("PyBridge::~PyBridge\n");
	}

	void PyBridge::_register_methods()
	{
		std::printf("PyBridge::_register_methods\n");

#define REGISTER_GODOT_METHOD(name) godot::register_method(#name, &PyBridge::name)
		REGISTER_GODOT_METHOD(get_random_action);
		REGISTER_GODOT_METHOD(get_dnn_action);
		REGISTER_GODOT_METHOD(remember);
		REGISTER_GODOT_METHOD(train_long_memory);
		REGISTER_GODOT_METHOD(train_short_memory);
#undef REGISTER_GODOT_METHOD
	}

	void PyBridge::_init()
	{
		std::printf("PyBridge::_init\n");
	}

	godot::Variant PyBridge::get_random_action()
	{
		return godot::Variant{ get_random_action_cpp() };
	}

	int PyBridge::get_random_action_cpp()
	{
		return randomize_int
		(	static_cast<int64_t>(NEXT_MOVE_DIRECTION::FORWARD)
		,	static_cast<int64_t>(NEXT_MOVE_DIRECTION::RIGHT)
		);
	}

	godot::Variant PyBridge::get_dnn_action(godot::Array state)
	{
		auto state_typed = godot_array_to_vector<int>(state);

		return get_dnn_action_cpp(state_typed);
	}

	int PyBridge::get_dnn_action_cpp(std::vector<int> const& state)
	{
		auto python_env = static_cast<CPythonInitializer*>(m_pythonEnvInitializer.get());
		return python_env->get_dnn_action(state);
	}

	void PyBridge::remember(godot::Array state, godot::Variant action, godot::Variant reward, godot::Array next_state, godot::Variant done)
	{
		auto state_typed = godot_array_to_vector<int>(state);
		auto action_typed = static_cast<int>(action);
		auto reward_typed = static_cast<int>(reward);
		auto next_state_typed = godot_array_to_vector<int>(next_state);
		auto done_typed = static_cast<bool>(done);

		remember_cpp(state_typed, action_typed, reward_typed, next_state_typed, done_typed);
	}

	void PyBridge::remember_cpp(std::vector<int> const& state, int action, int reward, std::vector<int> const& next_state, bool done)
	{
		auto python_env = static_cast<CPythonInitializer*>(m_pythonEnvInitializer.get());
		
		python_env->remember(state, action, reward, next_state, done);
	}

	void PyBridge::train_long_memory()
	{
		auto python_env = static_cast<CPythonInitializer*>(m_pythonEnvInitializer.get());

		python_env->train_long_memory();
	}

	void PyBridge::train_short_memory(godot::Array state, godot::Variant action, godot::Variant reward, godot::Array next_state, godot::Variant done)
	{
		auto state_typed = godot_array_to_vector<int>(state);
		auto action_typed = static_cast<int>(action);
		auto reward_typed = static_cast<int>(reward);
		auto next_state_typed = godot_array_to_vector<int>(next_state);
		auto done_typed = static_cast<bool>(done);

		train_short_memory_cpp(state_typed, action_typed, reward_typed, next_state_typed, done_typed);
	}
	void PyBridge::train_short_memory_cpp(std::vector<int> const& state, int action, int reward, std::vector<int> const& next_state, bool done)
	{
		auto python_env = static_cast<CPythonInitializer*>(m_pythonEnvInitializer.get());
		python_env->train_short_memory(state, action, reward, next_state, done);
	}
}
