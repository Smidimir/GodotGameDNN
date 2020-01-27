#include <pch.h>

#include "SnakeGame.h"

#include <classes/PyBridge.h>
#include <classes/SnakeMap.h>

#include <utils/Randomize.h>

namespace smidi
{
	/* constants */
	auto constexpr s_f_tick_delta = 0.01f;

	auto constexpr STATE_INDEX_DANGER_FORWARD = 0;
	auto constexpr STATE_INDEX_DANGER_LEFT    = 1;
	auto constexpr STATE_INDEX_DANGER_RIGHT   = 2;
	auto constexpr STATE_INDEX_MOVE_LEFT      = 3;
	auto constexpr STATE_INDEX_MOVE_UP        = 4;
	auto constexpr STATE_INDEX_MOVE_RIGHT     = 5;
	auto constexpr STATE_INDEX_MOVE_DOWN      = 6;
	auto constexpr STATE_INDEX_APPLE_LEFT     = 7;
	auto constexpr STATE_INDEX_APPLE_UP       = 8;
	auto constexpr STATE_INDEX_APPLE_RIGHT    = 9;
	auto constexpr STATE_INDEX_APPLE_DOWN     = 10;

	/* SnakeGame */
	SnakeGame::SnakeGame()
	{
		std::printf("SnakeGame::SnakeGame()\n");
	}

	SnakeGame::~SnakeGame()
	{
		std::printf("SnakeGame::~SnakeGame()\n");
	}

	void SnakeGame::_register_methods()
	{
		std::printf("SnakeGame::_register_methods\n");

#define REGISTER_GODOT_METHOD(name) { std::cout << "register method \"" << #name << "\"" << std::endl; godot::register_method(#name, &SnakeGame::name); }
		REGISTER_GODOT_METHOD(_ready);
		REGISTER_GODOT_METHOD(_process);
#undef REGISTER_GODOT_METHOD
	}

	void SnakeGame::_init()
	{
		std::printf("SnakeGame::_init\n");
	}

	void SnakeGame::_ready()
	{
		std::printf("SnakeGame::_ready\n");

		game_field = godot::Object::cast_to<SnakeMap>(
			get_node(godot::NodePath{ "TileFront" })
		);
		
		py_bridge = godot::Object::cast_to<PyBridge>(
			get_node(godot::NodePath{ "PyBridge" })
		);

		apple.tile_id = e_tile_id::APPLE;
	
		init_game();
	}

	//void SnakeGame::_process(godot::Variant fDt)
	//{
	//	process(fDt);
	//}

	void SnakeGame::_process(float fDt)
	{
		m_f_accumulative_delta += fDt;

		if (m_f_accumulative_delta >= s_f_tick_delta)
		{
			m_f_accumulative_delta -= s_f_tick_delta;
			tick_process();
		}
	}

	void SnakeGame::init_game()
	{
		std::printf("SnakeGame::init_game\n");

		e_snake_cur_move_direcion = CUR_MOVE_DIRECTION::RIGHT;
		e_snake_next_move_direction = NEXT_MOVE_DIRECTION::FORWARD;
	
		n_score = 0;
	
		replace_apple();
		create_snake();
	
		update_gfx();
	}

	void SnakeGame::replace_apple()
	{
		auto last_x_pos = game_field->get_map_size_x() - 1;
		auto last_y_pos = game_field->get_map_size_y() - 1;

		apple.pos_x = randomize_int(0, last_x_pos);
		apple.pos_y = randomize_int(0, last_y_pos);
	}

	void SnakeGame::create_snake()
	{
		snake.clear();

		snake.push_back(
			s_cell_info
			{	10
			,	10
			,	e_tile_id::SNAKE_HEAD
			}
		);
		snake.push_back(
			s_cell_info
			{	9
			,	10
			,	e_tile_id::SNAKE_BODY
			}
		);
		snake.push_back(
			s_cell_info
			{	8
			,	10
			,	e_tile_id::SNAKE_BODY
			}
		);
	}

	void SnakeGame::tick_process()
	{
		std::printf("SnakeGame::tick_process()\n");

		update_logic();
		update_gfx();

		reset_tick();
	}

	void SnakeGame::update_logic()
	{
		std::printf("SnakeGame::update_logic()\n");

		// auto play
		auto cur_state = get_cur_state();
		auto action = get_next_action(cur_state);
	
		apply_move(action);
	
		auto const& cur_head = snake.front();
	
		auto const cur_head_x = cur_head.pos_x;
		auto const cur_head_y = cur_head.pos_y;
	
		auto const next_head_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, e_snake_next_move_direction);
		auto const next_head_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, e_snake_next_move_direction);
	
		auto const apple_x = apple.pos_x;
		auto const apple_y = apple.pos_y;
	
		auto reward = 0;
		auto done = false;
	
		if
		(	(!cur_state[STATE_INDEX_DANGER_FORWARD] && e_snake_next_move_direction == NEXT_MOVE_DIRECTION::FORWARD)
		||	(!cur_state[STATE_INDEX_DANGER_LEFT]    && e_snake_next_move_direction == NEXT_MOVE_DIRECTION::LEFT)
		||	(!cur_state[STATE_INDEX_DANGER_RIGHT]   && e_snake_next_move_direction == NEXT_MOVE_DIRECTION::RIGHT)
		)
		{
			if (next_head_x == apple_x && next_head_y == apple_y)
			{
				handle_apple();
				reward += 10;
			}
			else
			{
				handle_move(next_head_x, next_head_y);
			}
			update_next_move_direction();
		}
		else
		{
			handle_move(next_head_x, next_head_y);
			handle_end();
			reward -= 10;
			done = true;
		}
	
		auto new_state = get_cur_state();
	
		py_bridge->train_short_memory_cpp(cur_state, static_cast<int>(action), reward, new_state, done);
		py_bridge->remember_cpp(cur_state, static_cast<int>(action), reward, new_state, done);
	
		if (done)
			py_bridge->train_long_memory();
	}

	void SnakeGame::update_gfx()
	{
		std::printf("SnakeGame::update_gfx\n");

		game_field->clear_map();

		game_field->apply_cell_info(apple);
		game_field->apply_cell_info_arr(snake);
	}

	void SnakeGame::reset_tick()
	{
		std::printf("SnakeGame::reset_tick()\n");

		e_snake_next_move_direction = NEXT_MOVE_DIRECTION::FORWARD;

		b_move_applied = false;
	}
	std::vector<int> SnakeGame::get_cur_state()
	{
		auto const& cur_head = snake.front();
	
		auto const cur_head_x = cur_head.pos_x;
		auto const cur_head_y = cur_head.pos_y;
	
		auto const next_forward_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION::FORWARD);
		auto const next_forward_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION::FORWARD);
	
		auto const next_left_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION::LEFT);
		auto const next_left_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION::LEFT);
	
		auto const next_right_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION::RIGHT);
		auto const next_right_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION::RIGHT);
	
		auto const apple_x = apple.pos_x;
		auto const apple_y = apple.pos_y;

		return
		{	is_hitting_wall(next_forward_x, next_forward_y) || is_hitting_snake(next_forward_x, next_forward_y)
		,	is_hitting_wall(next_left_x   , next_left_y   ) || is_hitting_snake(next_left_x   , next_left_y   )
		,	is_hitting_wall(next_right_x  , next_right_y  ) || is_hitting_snake(next_right_x  , next_right_y  )
		
		,	e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION::LEFT
		,	e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION::UP
		,	e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION::RIGHT
		,	e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION::DOWN
			
		,	apple_x < cur_head_x
		,	apple_y < cur_head_y
		,	apple_x > cur_head_x
		,	apple_y > cur_head_y
		};

	}
	int SnakeGame::get_x_position_after_move(int cur_x, CUR_MOVE_DIRECTION cur_move_direcion, NEXT_MOVE_DIRECTION next_move_direcion)
	{
		if (cur_move_direcion == CUR_MOVE_DIRECTION::LEFT)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::FORWARD)
				return cur_x - 1;
			else
				return cur_x;
		}
		else if (cur_move_direcion == CUR_MOVE_DIRECTION::UP)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::LEFT)
				return cur_x - 1;
			else if (next_move_direcion == NEXT_MOVE_DIRECTION::RIGHT)
				return cur_x + 1;
			else
				return cur_x;
		}
		else if (cur_move_direcion == CUR_MOVE_DIRECTION::RIGHT)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::FORWARD)
				return cur_x + 1;
			else
				return cur_x;
		}
		else // if (cur_move_direcion == CUR_MOVE_DIRECTION.DOWN)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::LEFT)
				return cur_x + 1;
			else if (next_move_direcion == NEXT_MOVE_DIRECTION::RIGHT)
				return cur_x - 1;
			else
				return cur_x;
		}
	}
	int SnakeGame::get_y_position_after_move(int cur_y, CUR_MOVE_DIRECTION cur_move_direcion, NEXT_MOVE_DIRECTION next_move_direcion)
	{
		if (cur_move_direcion == CUR_MOVE_DIRECTION::LEFT)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::LEFT)
				return cur_y + 1;
			else if (next_move_direcion == NEXT_MOVE_DIRECTION::RIGHT)
				return cur_y - 1;
			else
				return cur_y;
		}
		else if (cur_move_direcion == CUR_MOVE_DIRECTION::UP)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::FORWARD)
				return cur_y - 1;
			else
				return cur_y;
		}
		else if (cur_move_direcion == CUR_MOVE_DIRECTION::RIGHT)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::LEFT)
				return cur_y - 1;
			else if (next_move_direcion == NEXT_MOVE_DIRECTION::RIGHT)
				return cur_y + 1;
			else
				return cur_y;
		}
		else // else if (cur_move_direcion == CUR_MOVE_DIRECTION::DOWN)
		{
			if (next_move_direcion == NEXT_MOVE_DIRECTION::FORWARD)
				return cur_y + 1;
			else
				return cur_y;
		}
	}
	bool SnakeGame::is_hitting_wall(int pos_x, int pos_y)
	{
		auto const field_size_x = game_field->get_map_size_x();
		auto const field_size_y = game_field->get_map_size_y();

		return pos_x == -1 or pos_x == field_size_x or pos_y == -1 or pos_y == field_size_y;
	}
	bool SnakeGame::is_hitting_snake(int pos_x, int pos_y)
	{
		for (auto&& it : snake)
		{
			if (it.pos_x == pos_x && it.pos_y == pos_y)
				return &snake.back() != &it;
		}
		return false;
	}
	SnakeGame::NEXT_MOVE_DIRECTION SnakeGame::get_next_action(std::vector<int> const& state)
	{
		auto const epsilon = 150 - static_cast<int>(n_game_counter);
		if (randomize_int(0, 200) < epsilon)
			return static_cast<NEXT_MOVE_DIRECTION>(py_bridge->get_random_action_cpp());
		else
			return static_cast<NEXT_MOVE_DIRECTION>(py_bridge->get_dnn_action_cpp(state));
	}
	void SnakeGame::apply_move(NEXT_MOVE_DIRECTION next_move_direction)
	{
		if (!b_move_applied)
		{
			e_snake_next_move_direction = next_move_direction;
			b_move_applied = true;
		}
	}
	void SnakeGame::update_next_move_direction()
	{
		auto n_next_snake_cur_move_direcion = static_cast<int>(e_snake_cur_move_direcion);

		if (e_snake_next_move_direction == NEXT_MOVE_DIRECTION::LEFT)
			n_next_snake_cur_move_direcion -= 1;
		else if (e_snake_next_move_direction == NEXT_MOVE_DIRECTION::RIGHT)
			n_next_snake_cur_move_direcion += 1;
	
		if (n_next_snake_cur_move_direcion == static_cast<int>(CUR_MOVE_DIRECTION::LEFT) - 1)
			e_snake_cur_move_direcion = CUR_MOVE_DIRECTION::DOWN;
		else if (n_next_snake_cur_move_direcion == static_cast<int>(CUR_MOVE_DIRECTION::DOWN) + 1)
			e_snake_cur_move_direcion = CUR_MOVE_DIRECTION::LEFT;
		else
			e_snake_cur_move_direcion = static_cast<CUR_MOVE_DIRECTION>(n_next_snake_cur_move_direcion);
	}
	void SnakeGame::handle_move(int next_head_x, int next_head_y)
	{
		snake.pop_back();
		snake.front().tile_id = e_tile_id::SNAKE_BODY;

		snake.insert
		(	std::begin(snake)
		,	s_cell_info
			{	next_head_x
			,	next_head_y
			,	e_tile_id::SNAKE_HEAD
			}
		);
	}
	void SnakeGame::handle_apple()
	{
		snake.front().tile_id = e_tile_id::SNAKE_BODY;

		snake.insert
		(	std::begin(snake)
		,	s_cell_info
			{	apple.pos_x
			,	apple.pos_y
			,	e_tile_id::SNAKE_HEAD
			}
		);
	
		n_score += 1;
		replace_apple();
	}
	void SnakeGame::handle_end()
	{
		n_record = std::max(n_record, n_score);
		n_game_counter += 1;
	
		n_score_sum += n_score;
		auto const f_mean = static_cast<float>(n_score_sum) / static_cast<float>(n_game_counter);
	
		std::cout << n_game_counter << "," << n_score << "," << f_mean << std::endl;
	
		init_game();
	}
}
