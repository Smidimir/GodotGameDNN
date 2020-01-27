extends TileMap

const MAP_SIZE = {
	WIDTH  = 20,
	HEIGHT = 20
}

const TILE_ID = {
	SNAKE_BODY = 1,
	SNAKE_HEAD = 2,
	APPLE      = 3,
	EMPTY      = 4
}

static func get_tile_id_snake_body():
	return TILE_ID.SNAKE_BODY

static func get_tile_id_snake_head():
	return TILE_ID.SNAKE_HEAD

static func get_tile_id_snake_apple():
	return TILE_ID.APPLE

static func get_tile_id_empty():
	return TILE_ID.EMPTY

static func create_cell_info(pos_x: int, pos_y: int, tile_id):
	return {
		"pos_x"		: pos_x,
		"pos_y"		: pos_y,
		"tile_id"	: tile_id
	}

static func get_map_size_x():
	return MAP_SIZE.WIDTH

static func get_map_size_y():
	return MAP_SIZE.HEIGHT

static func get_map_size():
	return MAP_SIZE

func _ready():
	clear_map()

func fill_map_with(tile_id):
	for y in range(MAP_SIZE.HEIGHT):
		for x in range(MAP_SIZE.WIDTH):
			set_cell(x, y, tile_id)

func clear_map():
	fill_map_with(TILE_ID.EMPTY)

func apply_cell_info(cell_info):
	set_cell(cell_info.pos_x, cell_info.pos_y, cell_info.tile_id)

func apply_cell_info_arr(cell_info_arr):
	for cell_info in cell_info_arr:
		apply_cell_info(cell_info)
