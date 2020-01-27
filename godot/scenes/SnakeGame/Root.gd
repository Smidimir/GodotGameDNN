extends Node2D

const f_tick_delta = 0.01

enum CUR_MOVE_DIRECTION {
	LEFT  = 0,
	UP    = 1,
	RIGHT = 2,
	DOWN  = 3
}

enum NEXT_MOVE_DIRECTION {
	FORWARD = 0,
	LEFT    = 1,
	RIGHT   = 2
}

enum ACTION_TYPE {
	MOVE,
	GROWTH,
	FAIL
}

# nodes
var game_field
var py_bridge

# game
var f_accumulative_delta = 0

var n_score = 0
var n_record = 0
var n_game_counter = 0
var n_score_sum = 0

# game logic
var e_snake_cur_move_direcion = CUR_MOVE_DIRECTION.RIGHT
var e_snake_next_move_direction = NEXT_MOVE_DIRECTION.FORWARD
var b_move_applied = false

var snake
var apple

static func randomize_int(from, to):
	randomize()
	return round(rand_range(from, to))

const STATE_INDEX_DANGER_FORWARD = 0
const STATE_INDEX_DANGER_LEFT    = 1
const STATE_INDEX_DANGER_RIGHT   = 2
const STATE_INDEX_MOVE_LEFT      = 3
const STATE_INDEX_MOVE_UP        = 4
const STATE_INDEX_MOVE_RIGHT     = 5
const STATE_INDEX_MOVE_DOWN      = 6
const STATE_INDEX_APPLE_LEFT     = 7
const STATE_INDEX_APPLE_UP       = 8
const STATE_INDEX_APPLE_RIGHT    = 9
const STATE_INDEX_APPLE_DOWN     = 10
static func make_state_array(
		danger_forward: bool, danger_left: bool, danger_right: bool,
		move_left     : bool, move_up    : bool, move_right  : bool, move_down : bool,
		apple_left    : bool, apple_up   : bool, apple_right : bool, apple_down: bool
	):
	return [
		danger_forward, danger_left, danger_right,
		move_left     , move_up    , move_right  , move_down,
		apple_left    , apple_up   , apple_right , apple_down
	]

static func get_x_position_after_move(cur_x, cur_move_direcion, next_move_direcion):
	if cur_move_direcion == CUR_MOVE_DIRECTION.LEFT:
		if next_move_direcion == NEXT_MOVE_DIRECTION.FORWARD:
			return cur_x - 1
		else:
			return cur_x
	elif cur_move_direcion == CUR_MOVE_DIRECTION.UP:
		if next_move_direcion == NEXT_MOVE_DIRECTION.LEFT:
			return cur_x - 1
		if next_move_direcion == NEXT_MOVE_DIRECTION.RIGHT:
			return cur_x + 1
		else:
			return cur_x
	elif cur_move_direcion == CUR_MOVE_DIRECTION.RIGHT:
		if next_move_direcion == NEXT_MOVE_DIRECTION.FORWARD:
			return cur_x + 1
		else:
			return cur_x
	else: # elif cur_move_direcion == CUR_MOVE_DIRECTION.DOWN:
		if next_move_direcion == NEXT_MOVE_DIRECTION.LEFT:
			return cur_x + 1
		if next_move_direcion == NEXT_MOVE_DIRECTION.RIGHT:
			return cur_x - 1
		else:
			return cur_x

static func get_y_position_after_move(cur_y, cur_move_direcion, next_move_direcion):
	if cur_move_direcion == CUR_MOVE_DIRECTION.LEFT:
		if next_move_direcion == NEXT_MOVE_DIRECTION.LEFT:
			return cur_y + 1
		if next_move_direcion == NEXT_MOVE_DIRECTION.RIGHT:
			return cur_y - 1
		else:
			return cur_y
	elif cur_move_direcion == CUR_MOVE_DIRECTION.UP:
		if next_move_direcion == NEXT_MOVE_DIRECTION.FORWARD:
			return cur_y - 1
		else:
			return cur_y
	elif cur_move_direcion == CUR_MOVE_DIRECTION.RIGHT:
		if next_move_direcion == NEXT_MOVE_DIRECTION.LEFT:
			return cur_y - 1
		if next_move_direcion == NEXT_MOVE_DIRECTION.RIGHT:
			return cur_y + 1
		else:
			return cur_y
	else: # elif cur_move_direcion == CUR_MOVE_DIRECTION.DOWN:
		if next_move_direcion == NEXT_MOVE_DIRECTION.FORWARD:
			return cur_y + 1
		else:
			return cur_y

func _ready():
	game_field = $TileFront
	py_bridge = $PyBridge
	
	apple = game_field.create_cell_info(
		0, 1,
		game_field.get_tile_id_snake_apple()
	)
	
	init_game()

func _process(delta):
	f_accumulative_delta += delta
	
	if f_accumulative_delta >= f_tick_delta:
		f_accumulative_delta -= f_tick_delta
		tick_process()

func create_snake():
	return [
		game_field.create_cell_info(
			10,
			10,
			game_field.get_tile_id_snake_head()
		),
		game_field.create_cell_info(
			9,
			10,
			game_field.get_tile_id_snake_body()
		),
		game_field.create_cell_info(
			8,
			10,
			game_field.get_tile_id_snake_body()
		)
	]

func init_game():
	e_snake_cur_move_direcion = CUR_MOVE_DIRECTION.RIGHT
	e_snake_next_move_direction = NEXT_MOVE_DIRECTION.FORWARD
	
	n_score = 0
	
	replace_apple()
	snake = create_snake();
	
	update_gfx()

func show_score():
	print("SCORE: ", n_score)

func tick_process():
	update_logic()
	update_gfx()

	reset_tick()

func reset_tick():
	e_snake_next_move_direction = NEXT_MOVE_DIRECTION.FORWARD
	
	b_move_applied = false

func _input(event):
	if event.is_action_pressed("ui_left"):
		process_input_left()
	if event.is_action_pressed("ui_right"):
		process_input_right()


func process_input_left():
	apply_move(NEXT_MOVE_DIRECTION.LEFT)

func process_input_right():
	apply_move(NEXT_MOVE_DIRECTION.RIGHT)

func get_next_action_type():
	return ACTION_TYPE.MOVE

func replace_apple():
	var last_x_pos = game_field.get_map_size_x() - 1
	var last_y_pos = game_field.get_map_size_y() - 1
	
	apple.pos_x = randomize_int(0, last_x_pos)
	apple.pos_y = randomize_int(0, last_y_pos)

func is_hitting_wall(pos_x, pos_y):
	var field_size_x = game_field.get_map_size_x()
	var field_size_y = game_field.get_map_size_y()

	return pos_x == -1 or pos_x == field_size_x or pos_y == -1 or pos_y == field_size_y

func is_hitting_snake(pos_x, pos_y):
	for it in snake:
		if it.pos_x == pos_x and it.pos_y == pos_y:
			return snake.back() != it
	return false

func get_cur_state():
	var cur_head = snake.front()
	
	var cur_head_x = cur_head.pos_x
	var cur_head_y = cur_head.pos_y
	
	var next_forward_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION.FORWARD)
	var next_forward_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION.FORWARD)
	
	var next_left_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION.LEFT)
	var next_left_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION.LEFT)
	
	var next_right_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION.RIGHT)
	var next_right_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, NEXT_MOVE_DIRECTION.RIGHT)
	
	var apple_x = apple.pos_x
	var apple_y = apple.pos_y
	
	return make_state_array(
		is_hitting_wall(next_forward_x, next_forward_y) or is_hitting_snake(next_forward_x, next_forward_y),
		is_hitting_wall(next_left_x   , next_left_y   ) or is_hitting_snake(next_left_x   , next_left_y   ),
		is_hitting_wall(next_right_x  , next_right_y  ) or is_hitting_snake(next_right_x  , next_right_y  ),
		
		e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION.LEFT,
		e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION.UP,
		e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION.RIGHT,
		e_snake_cur_move_direcion ==  CUR_MOVE_DIRECTION.DOWN,
		
		apple_x < cur_head_x,
		apple_y < cur_head_y,
		apple_x > cur_head_x,
		apple_y > cur_head_y
	)

func apply_move(next_move_direction):
	if not b_move_applied:
		e_snake_next_move_direction = next_move_direction
		b_move_applied = true

func update_next_move_direction():
	if e_snake_next_move_direction == NEXT_MOVE_DIRECTION.LEFT:
			e_snake_cur_move_direcion -= 1
	elif e_snake_next_move_direction == NEXT_MOVE_DIRECTION.RIGHT:
			e_snake_cur_move_direcion += 1
	
	if e_snake_cur_move_direcion == CUR_MOVE_DIRECTION.LEFT - 1:
		e_snake_cur_move_direcion = CUR_MOVE_DIRECTION.DOWN
	elif e_snake_cur_move_direcion == CUR_MOVE_DIRECTION.DOWN + 1:
		e_snake_cur_move_direcion = CUR_MOVE_DIRECTION.LEFT

func handle_move(next_head_x, next_head_y):
	snake.pop_back()
	snake.front().tile_id = game_field.get_tile_id_snake_body()
	snake.push_front(
		game_field.create_cell_info(
			next_head_x,
			next_head_y,
			game_field.get_tile_id_snake_head()
		)
	)

func handle_apple():
	snake.front().tile_id = game_field.get_tile_id_snake_body()
	snake.push_front(
		game_field.create_cell_info(
			apple.pos_x,
			apple.pos_y,
			game_field.get_tile_id_snake_head()
		)
	)
	
	n_score += 1
	replace_apple()
	
#	show_score()

func handle_end():
	n_record = max(n_record, n_score)
	n_game_counter += 1
	
	n_score_sum += n_score
	var f_mean: float = float(n_score_sum) / float(n_game_counter)
	
	print(n_game_counter, ",", n_score, ",", f_mean)
	
	init_game()

func get_next_action(state):
	var epsilon = 150 - n_game_counter
	if randomize_int(0, 200) < epsilon:
		return py_bridge.get_random_action()
	else:
		return py_bridge.get_dnn_action(state)


func update_logic():
	# auto play
	var cur_state = get_cur_state()
	var action = get_next_action(cur_state)
	
	apply_move(action)
	
	var cur_head = snake.front()
	
	var cur_head_x = cur_head.pos_x
	var cur_head_y = cur_head.pos_y
	
	var next_head_x = get_x_position_after_move(cur_head_x, e_snake_cur_move_direcion, e_snake_next_move_direction)
	var next_head_y = get_y_position_after_move(cur_head_y, e_snake_cur_move_direcion, e_snake_next_move_direction)
	
	var apple_x = apple.pos_x
	var apple_y = apple.pos_y
	
	var reward = 0
	var done = false
	
	if	not cur_state[STATE_INDEX_DANGER_FORWARD] and e_snake_next_move_direction == NEXT_MOVE_DIRECTION.FORWARD or \
		not cur_state[STATE_INDEX_DANGER_LEFT]    and e_snake_next_move_direction == NEXT_MOVE_DIRECTION.LEFT    or \
		not cur_state[STATE_INDEX_DANGER_RIGHT]   and e_snake_next_move_direction == NEXT_MOVE_DIRECTION.RIGHT:
		if next_head_x == apple_x and next_head_y == apple_y:
			handle_apple()
			reward += 10
		else:
			handle_move(next_head_x, next_head_y)
		update_next_move_direction()
	else:
		handle_move(next_head_x, next_head_y)
		handle_end()
		reward -= 10
		done = true
	
	var new_state = get_cur_state()
	
	py_bridge.train_short_memory(cur_state, action, reward, new_state, done)
	py_bridge.remember(cur_state, action, reward, new_state, done)
	
	if done:
		py_bridge.train_long_memory()

func update_gfx():
	game_field.clear_map();
	game_field.apply_cell_info(apple)
	game_field.apply_cell_info_arr(snake)