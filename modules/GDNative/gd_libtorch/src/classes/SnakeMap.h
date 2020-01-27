#pragma once

#include <core/Godot.hpp>
#include <gen/TileMap.hpp>

#include <vector>

namespace smidi
{
	enum class e_tile_id : int
	{
		SNAKE_BODY = 1,
		SNAKE_HEAD = 2,
		APPLE      = 3,
		EMPTY      = 4
	};

	struct s_cell_info
	{
		int       pos_x   = 0;
		int       pos_y   = 0;
		e_tile_id tile_id = e_tile_id::EMPTY;
	};

	/**
	 * SnakeMap
	 */

	class SnakeMap
		: public godot::TileMap
	{
		GODOT_CLASS(SnakeMap, TileMap);
	public:
		SnakeMap();
		~SnakeMap();

		static void _register_methods();

		void _init();

		void _ready();

		int get_map_size_x() const;
		int get_map_size_y() const;

		void clear_map();
		void fill_map_with(e_tile_id tile_id);
		void apply_cell_info(s_cell_info const& cell_info);
		void apply_cell_info_arr(std::vector<s_cell_info> const& cell_info_arr);

	private:

	};
}