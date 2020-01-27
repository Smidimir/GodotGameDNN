#pragma once

#include <classes/SnakeMap.h>

#include <core/Godot.hpp>
#include <gen/Node2D.hpp>

#include <vector>
#include <memory>

namespace smidi
{
	class PyBridge;
	class SnakeMap;

	class SnakeGame
		: public godot::Node2D
	{
		GODOT_CLASS(SnakeGame, Node2D);

		enum class CUR_MOVE_DIRECTION : int
		{
			LEFT  = 0,
			UP    = 1,
			RIGHT = 2,
			DOWN  = 3
		};

		enum class NEXT_MOVE_DIRECTION : int
		{
			FORWARD = 0,
			LEFT    = 1,
			RIGHT   = 2
		};

		enum class ACTION_TYPE : int
		{
			MOVE,
			GROWTH,
			FAIL
		};

	public:
		SnakeGame();
		~SnakeGame();

		static void _register_methods();

		void _init();

		void _ready();
		void _process(float fDt);

		void init_game();
		void replace_apple();
		void create_snake();

		void tick_process();
		void update_logic();
		void update_gfx();
		void reset_tick();

		std::vector<int> get_cur_state();
		int get_x_position_after_move(int cur_x, CUR_MOVE_DIRECTION cur_move_direcion, NEXT_MOVE_DIRECTION next_move_direcion);
		int get_y_position_after_move(int cur_y, CUR_MOVE_DIRECTION cur_move_direcion, NEXT_MOVE_DIRECTION next_move_direcion);
		bool is_hitting_wall(int pos_x, int pos_y);
		bool is_hitting_snake(int pos_x, int pos_y);
		NEXT_MOVE_DIRECTION get_next_action(std::vector<int> const& state);
		void apply_move(NEXT_MOVE_DIRECTION next_move_direction);
		void update_next_move_direction();

		void handle_move(int next_head_x, int next_head_y);
		void handle_apple();
		void handle_end();

	private:
		// nodes
		SnakeMap* game_field;
		PyBridge* py_bridge;

		// game
		float m_f_accumulative_delta = 0.f;

		std::uint32_t n_score = 0;
		std::uint32_t n_record = 0;
		std::uint32_t n_game_counter = 0;
		std::uint32_t n_score_sum = 0;

		// game logic
		CUR_MOVE_DIRECTION e_snake_cur_move_direcion = CUR_MOVE_DIRECTION::RIGHT;
		NEXT_MOVE_DIRECTION e_snake_next_move_direction = NEXT_MOVE_DIRECTION::FORWARD;
		bool b_move_applied = false;

		std::vector<s_cell_info> snake;
		s_cell_info apple;
	};
}