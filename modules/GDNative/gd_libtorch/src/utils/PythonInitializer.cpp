#include <pch.h>
#include "PythonInitializer.h"

smidi::CPythonInitializer::CPythonInitializer()
	: m_module_python_env{ pybind11::module::import("gdignore.python.modules.env_init") }
	, m_module_agent{ pybind11::module::import("AgentNoUI") }
	, m_agent{ m_module_agent.attr("DQNAgent_train")() }
{
	std::printf("CPythonInitializer::CPythonInitializer()\n");
}

smidi::CPythonInitializer::~CPythonInitializer()
{
	std::printf("CPythonInitializer::~CPythonInitializer()\n");
}

int smidi::CPythonInitializer::get_dnn_action(std::vector<int> const& state)
{
	auto action = m_agent.attr("get_dnn_action")(state).cast<std::vector<int>>();

	auto it = std::find(std::begin(action), std::end(action), 1);

	return std::distance(std::begin(action), it);
}

void smidi::CPythonInitializer::remember(std::vector<int> const& state, int action, int reward, std::vector<int> const& next_state, bool done)
{
	auto action_vec = std::vector<int>{0, 0, 0};
	action_vec[action] = 1;

	m_agent.attr("remember")(state, action_vec, reward, next_state, done);
}

void smidi::CPythonInitializer::train_long_memory()
{
	m_agent.attr("train_long_memory")();
}

void smidi::CPythonInitializer::train_short_memory(std::vector<int> const& state, int action, int reward, std::vector<int> const& next_state, bool done)
{
	auto action_vec = std::vector<int>{ 0, 0, 0 };
	action_vec[action] = 1;

	m_agent.attr("train_short_memory")(state, action_vec, reward, next_state, done);
}
