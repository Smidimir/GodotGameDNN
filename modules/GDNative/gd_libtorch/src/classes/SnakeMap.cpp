#include <pch.h>

#include "SnakeMap.h"

namespace smidi
{
	/* map size */
	auto constexpr s_n_map_width  = 20;
	auto constexpr s_n_map_height = 20;

	/* SnakeGame */
	SnakeMap::SnakeMap()
	{
		std::printf("SnakeMap::SnakeGame()\n");
	}

	SnakeMap::~SnakeMap()
	{
		std::printf("SnakeMap::~SnakeGame()\n");
	}

	void SnakeMap::_register_methods()
	{
		std::printf("SnakeMap::_register_methods\n");

#define REGISTER_GODOT_METHOD(name) { std::cout << "register method \"" << #name << "\"" << std::endl; godot::register_method(#name, &SnakeMap::name); }
		REGISTER_GODOT_METHOD(_ready);
#undef REGISTER_GODOT_METHOD
	}

	void SnakeMap::_init()
	{
		std::printf("SnakeMap::_init\n");
	}

	void SnakeMap::_ready()
	{
		std::printf("SnakeMap::_ready\n");

		clear_map();
	}
	int SnakeMap::get_map_size_x() const
	{
		return s_n_map_width;
	}
	int SnakeMap::get_map_size_y() const
	{
		return s_n_map_height;
	}
	void SnakeMap::clear_map()
	{
		fill_map_with(e_tile_id::EMPTY);
	}
	void SnakeMap::fill_map_with(e_tile_id tile_id)
	{
		for (std::size_t y = 0; y < s_n_map_height; ++y)
			for (std::size_t x = 0; x < s_n_map_width; ++x)
				set_cell(x, y, static_cast<int>(tile_id));
	}
	void SnakeMap::apply_cell_info(s_cell_info const& cell_info)
	{
		set_cell(cell_info.pos_x, cell_info.pos_y, static_cast<int>(cell_info.tile_id));
	}
	void SnakeMap::apply_cell_info_arr(std::vector<s_cell_info> const& cell_info_arr)
	{
		for (s_cell_info const& cell_info : cell_info_arr)
			apply_cell_info(cell_info);
	}
}
