#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cassert>
#include <intrin.h>
#include "tetromino.hpp"

using namespace std;

const int scrW = 700;
const int scrH = 1000;

// width and height of playing field
const int fW = 10;
const int fH = 22;

const float ticks_per_sec = 60;
const float frames_per_sec = 60;

// count every tick btw drops
int tick_counter = 0;
// how often tetromino should drop by 1 cell
int fall_interval = 30;

// tile size
const int tile = 26;

// is used in bag randomizer
int arr[] = { 0, 1, 2, 3, 4, 5, 6 };
int choose_from = 7;

// used to check if tetromino was dropped before the tick drop
int older_pos_y = 0;

// used to clear line
int row_index = 0;
int rows_to_clear = 0;

bool edit_mode = false;

struct Vec2Int {
	int x;
	int y;
};

enum TileState {
	EMPTY,
	HAS_VALUE = 2,
	EDITED,
};

struct Tile {
	TileState state;
	Color tile_color;
};

// Map is a struct
// Map contains 2D array of tiles - playing field (is private,
// you can access it only by using the operator[].
struct Map {
private:
	Tile map[fH][fW];
public:
	Tile& operator[](Vec2Int v) {
		return map[v.y][v.x];
	}
};
Map field;

struct Tetromino {
	Color color;
	TetrominoType type;
	Vector2 p = { 0,0 };
	int local_template[4][4];
};
Tetromino tetromino;
Tetromino t_ghost;

// ---------------------------------------------------------------------------

// the state of each tile on the map at the start
void init_field_state() {
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {
			field[{x,y}].state = EMPTY;
		}
	}
}

// locking tetromino
void lock_tetromino(Tetromino t) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (tetromino.local_template[y][x] == 1) {
				field[{int(x + t.p.x), int(y + t.p.y)}].state = HAS_VALUE;
				field[{int(x + t.p.x), int(y + t.p.y)}].tile_color = t.color;
			}
		}
	}
}

TetrominoType rand_type() {
	// total length of an array (assumed to be 7)
	int length = sizeof(arr) / sizeof(arr[0]);

	// select an index for tetromino:
	// val from 0 to (num of allowed elements)
	int n = GetRandomValue(0, choose_from - 1);

	// get the tetromino value
	int tetromino_num = arr[n];

	// remove the selected tetromino from the list by shifting elements left
	// and place the used tetromino at the end of the array
	for (int i = n + 1; i <= length - 1; i++) {
		arr[i - 1] = arr[i];
	}
	arr[length - 1] = tetromino_num;

	// decrease the pool of available tetrominoes unless only one remains,
	// then reset
	if (choose_from > 1) {
		choose_from--;
	}
	else {
		choose_from = 7;
	}

	return TetrominoType(tetromino_num);
}

void create_tetromino() {
	tetromino.type = rand_type(); 
	tetromino.color = colors[tetromino.type];
	tetromino.p = { 3,0 };
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			tetromino.local_template[y][x] = tetrominoShapes[tetromino.type][y][x];
		}
	}

	//ghost 
	t_ghost.p = tetromino.p;
	t_ghost.type = tetromino.type;
	t_ghost.color = GHOST_GRAY;
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			t_ghost.local_template[y][x] = tetromino.local_template[y][x];
		}
	}
}

// Rotates by 90 
void rotate(Tetromino* t, float deg) {
	int local_tpl_copy[4][4];

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			local_tpl_copy[y][x] = t->local_template[y][x];
			t->local_template[y][x] = 0;
		}
	}

	Vector2 centers[7] = {
		{ 1.5, 1.5 },
		{ 1,1 },
		{ 1.5, 0.5 },
		{ 1,1 },
		{ 1,1 },
		{ 1,1 },
		{ 1,1 },
	};

	Vector2 center = centers[t->type];

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (local_tpl_copy[y][x] == 1) {
				Vector2 cell = { float(x), float(y) };
				Vector2 rel_center = Vector2Subtract(cell, center);
				Vector2 rotated_v = Vector2Rotate(rel_center, deg * DEG2RAD);
				cell = Vector2Add(rotated_v, center);
				t->local_template[int(round(cell.y))][int(round(cell.x))] = 1;
			}
		}
	}
}

bool can_place(Tetromino t, int pos_x, int pos_y) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (t.local_template[y][x] != 1) {
				continue;
			}
			if (pos_x + x < 0 || pos_x + x >= fW) {
				return false;
			}
			if (pos_y + y >= fH) {
				return false;
			}
			int tile_st = int(field[{pos_x + x, pos_y + y}].state);
			assert(tile_st >= 0 && tile_st <= 3);
			if (tile_st != 0) {
				return false;
			}
		}
	}
	return true;
}

bool can_place_ghost( int pos_x, int pos_y ) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (tetromino.local_template[y][x] != 1) {
				continue;
			}
			if (pos_x + x < 0 || pos_x + x >= fW) {
				return false;
			}
			if (pos_y + y >= fH) {
				return false;
			}
			int tile_st = int(field[{pos_x + x, pos_y + y}].state);
			assert(tile_st >= 0 && tile_st <= 3);
			if (tile_st != 0) {
				return false;
			}
		}
	}
	return true;
}

double start_holding = 0.f;
double initial_delay = 0.19f;

bool moving_left = false;
bool moving_right = false;
bool moving_down = false;

void input_handler() {
	int copy_pos_x = 0;
	int copy_pos_y = 0;

	if (IsKeyPressed(KEY_LEFT)) {
		start_holding = GetTime();
		moving_left = true;
		copy_pos_x = tetromino.p.x - 1;
		copy_pos_y = tetromino.p.y;
		if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
			tetromino.p.x -= 1;
		}
 	}
	if (IsKeyReleased(KEY_LEFT)) {
		moving_left = false;
	}
	if (moving_left && GetTime() - start_holding >= initial_delay) {
		int move_interval = 2;
		if (tick_counter % move_interval == 0) {
			copy_pos_x = tetromino.p.x - 1;
			copy_pos_y = tetromino.p.y;
			if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
				tetromino.p.x -= 1;
			}
		}
	}

	if (IsKeyPressed(KEY_RIGHT)) {
		start_holding = GetTime();
		moving_right = true;
		copy_pos_x = tetromino.p.x + 1;
		copy_pos_y = tetromino.p.y;
		if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
			tetromino.p.x += 1;
		}
	}
	if (IsKeyReleased(KEY_RIGHT)) {
		moving_right = false;
	}
	if (moving_right && GetTime() - start_holding >= initial_delay) {
		int move_interval = 2;
		if (tick_counter % move_interval == 0) {
			copy_pos_x = tetromino.p.x + 1;
			copy_pos_y = tetromino.p.y;
			if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
				tetromino.p.x += 1;
			}
		}
	}

	if (IsKeyPressed(KEY_DOWN)) {
		start_holding = GetTime();
		moving_down = true;
		older_pos_y = tetromino.p.y;
		copy_pos_x = tetromino.p.x;
		copy_pos_y = tetromino.p.y + 1;
		if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
			tetromino.p.y += 1;
		}
		else {
			lock_tetromino(tetromino);
			create_tetromino();
		}
	}
	if (IsKeyReleased(KEY_DOWN)) {
		moving_down = false;
	}
	if (moving_down && GetTime() - start_holding >= initial_delay) {
		int move_interval = 2;
		if (tick_counter % move_interval == 0) {
			copy_pos_x = tetromino.p.x;
			copy_pos_y = tetromino.p.y + 1;
			if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
				tetromino.p.y += 1;
			}
			else {
				lock_tetromino(tetromino);
				create_tetromino();
			}
		}
	}

	if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_X)) {
		Tetromino t_copy = tetromino;
		copy_pos_x = t_copy.p.x;
		copy_pos_y = t_copy.p.y;
		rotate(&t_copy, 90.f);
		if (can_place(t_copy, copy_pos_x, copy_pos_y)) {
			rotate(&tetromino, 90.f);
		}
		else if (can_place(t_copy, copy_pos_x + 1, copy_pos_y)) {
			tetromino.p.x += 1;
			rotate(&tetromino, 90.f);
		}
		else if (can_place(t_copy, copy_pos_x - 1, copy_pos_y)) {
			tetromino.p.x -= 1;
			rotate(&tetromino, 90.f);
		}
		else if (can_place(t_copy, copy_pos_x, copy_pos_y - 1)) {
			tetromino.p.y -= 1; 
			rotate(&tetromino, 90.f);
		}
		else if (can_place(t_copy, copy_pos_x + 1, copy_pos_y - 1)) {
			tetromino.p.x += 1;
			tetromino.p.y -= 1;
			rotate(&tetromino, 90.f);
		}
		else if (can_place(t_copy, copy_pos_x - 1, copy_pos_y - 1)) {
			tetromino.p.x -= 1;
			tetromino.p.y -= 1;
			rotate(&tetromino, 90.f);
		}
		else if (can_place(t_copy, copy_pos_x + 2, copy_pos_y - 2)) {
			tetromino.p.x += 2;
			tetromino.p.y -= 2;
			rotate(&tetromino, 90.f);
		}
		else if (can_place(t_copy, copy_pos_x - 2, copy_pos_y - 2)) {
			tetromino.p.x -= 2;
			tetromino.p.y -= 2;
			rotate(&tetromino, 90.f);
		}
	}

	if (IsKeyPressed(KEY_Z)) {
		Tetromino t_copy = tetromino;
		copy_pos_x = t_copy.p.x;
		copy_pos_y = t_copy.p.y;
		rotate(&t_copy, -90.f);
		if (can_place(t_copy, copy_pos_x, copy_pos_y)) {
			rotate(&tetromino, -90.f);
		}
		else if (can_place(t_copy, copy_pos_x + 1, copy_pos_y)) {
			tetromino.p.x += 1;
			rotate(&tetromino, -90.f);
		}
		else if (can_place(t_copy, copy_pos_x - 1, copy_pos_y)) {
			tetromino.p.x -= 1;
			rotate(&tetromino, -90.f);
		}
		else if (can_place(t_copy, copy_pos_x, copy_pos_y - 1)) {
			tetromino.p.y -= 1; 
			rotate(&tetromino, -90.f);
		}
		else if (can_place(t_copy, copy_pos_x + 1, copy_pos_y - 1)) {
			tetromino.p.x += 1;
			tetromino.p.y -= 1;
			rotate(&tetromino, -90.f);
		}
		else if (can_place(t_copy, copy_pos_x - 1, copy_pos_y - 1)) {
			tetromino.p.x -= 1;
			tetromino.p.y -= 1;
			rotate(&tetromino, -90.f);
		}
		else if (can_place(t_copy, copy_pos_x + 2, copy_pos_y - 2)) {
			tetromino.p.x += 2;
			tetromino.p.y -= 2;
			rotate(&tetromino, -90.f);
		}
		else if (can_place(t_copy, copy_pos_x - 2, copy_pos_y - 2)) {
			tetromino.p.x -= 2;
			tetromino.p.y -= 2;
			rotate(&tetromino, -90.f);
		}
	}

	if (IsKeyPressed(KEY_SPACE)) {
		copy_pos_x = tetromino.p.x;
		copy_pos_y = tetromino.p.y;
		while(can_place(tetromino, copy_pos_x, copy_pos_y + 1)){
			tetromino.p.y += 1;
			copy_pos_y = tetromino.p.y;
		}
		lock_tetromino(tetromino);
		create_tetromino();
	}
}

// -----------------------------RENDER FUNCTIONS-------------------------------------

Vector2 convert_to_px(Vector2 v) {
	Vector2 v_px = { v.x * tile, v.y * tile };
	//centerize
	float w = scrW/2 - fW/2 * tile;
	float h = scrH/2 - fH/2 * tile;
	return { v_px.x + w , v_px.y + h };
}

// Check tile's state to render it
// and offset playing field to the center of the screen while rendering
void render_map() {
	Vector2 t_size = { tile, tile };
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {
			Vector2 v = { float(x), float(y) };
			if (field[{x, y}].state == EMPTY) {
				DrawRectangleV(convert_to_px(v), t_size, BLACK);
			}
			if (field[{x, y}].state == HAS_VALUE) {
				Color t_col = field[{x, y}].tile_color;
				DrawRectangleV(convert_to_px(v), t_size, t_col);
			}
			if (field[{x, y}].state == EDITED) {
				DrawRectangleV(convert_to_px(v), t_size, GRAY);
			}
		}
	}
}

void render_grid() {
	for (int y = 2; y <= fH; y++) {
		DrawLineV(convert_to_px({ 0.f, float(y) }), convert_to_px({ fW, float(y) }), GRID_GRAY);
	}
	for (int x = 0; x <= fW; x++) {
		DrawLineV(convert_to_px({ float(x), 2.f }), convert_to_px({ float(x), fH }), GRID_GRAY);
	}
}


void render_tetromino(Tetromino const& t, Tetromino &ghost) {
	Vector2 t_size = { tile, tile };
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (tetromino.local_template[y][x] == 1) {
				Vector2 ghost_pos = convert_to_px(Vector2Add(ghost.p, { float(x), float(y) }));
				int xx = ghost.p.x;
				int yy = ghost.p.y;
				DrawRectangleV(ghost_pos, t_size, t_ghost.color);

				Vector2 piece_pos = convert_to_px(Vector2Add(t.p, { float(x), float(y) }));
				int tx = t.p.x;
				int ty = t.p.y;
				DrawRectangleV(piece_pos, t_size, tetromino.color);
			}
		}
	}
}


void render_data(bool game_mode) {
	DrawText("Edit mode: ", 50, 50, 20, WHITE);
	DrawText(TextFormat("%d", edit_mode ? 1 : 0), 155, 50, 20, WHITE);
}

// ----------------------------------------------------------------------------------------------

void edit_map() {
	float w = scrW/2 - fW/2 * tile;
	float h = scrH/2 - fH/2 * tile;
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		Vector2 mouse_px = GetMousePosition();
		mouse_px = { mouse_px.x - w, mouse_px.y - h };
		cout << mouse_px.x << ", " << mouse_px.y << endl;
		// Convert mose position from pixels to tiles
		Vector2 mouse_tile = { floor(mouse_px.x / tile), floor(mouse_px.y / tile) };
		cout << mouse_tile.x << ", " << mouse_tile.y << endl;

		field[{int(mouse_tile.x), int(mouse_tile.y)}].state = EDITED;
	}
}

void tick() {
	// can gravity drop tetronmino by 1 now?
	if (tick_counter % fall_interval == 0) {
		if (older_pos_y == tetromino.p.y) {
			if (can_place(tetromino, tetromino.p.x, tetromino.p.y + 1)) {
				tetromino.p.y += 1;
			}
			else {
				lock_tetromino(tetromino);
				create_tetromino();
			}
		}
		older_pos_y = tetromino.p.y;
	}
}

void clear_line(int row) {
	for (int x = 0; x < fW; x++) {
		field[{x, row}].state = EMPTY;
	}
	for (int y = row - 1; y >= 0; y--) {
		for (int x = fW - 1; x >= 0; x--) {
			if (field[{x, y}].state == HAS_VALUE || field[{x, y}].state == EDITED) {
				field[{x, y + 1}].state = field[{x, y}].state;
				field[{x, y + 1}].tile_color = field[{x, y}].tile_color;
				field[{x, y}].state = EMPTY;
			}
		}
	}
}

void check_row_clear() {
	int cells_to_clear = 0;
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {
			if (field[{x, y}].state == EMPTY) {
				cells_to_clear = 0;
				break;
			}
			if (field[{x, y}].state == HAS_VALUE || field[{x, y}].state == EDITED) {
				cells_to_clear++;
			}
		}
		if (cells_to_clear == fW) {
			row_index = y; 
			cells_to_clear = 0;
			clear_line(row_index);
			y--;
		}
	}
}

void render() {
	BeginDrawing();
		ClearBackground(BLACK);
			render_map();
			render_grid();
			render_tetromino(tetromino, t_ghost);
			render_data(edit_mode);
	EndDrawing();
}

int main() {
	//Initialization
	InitWindow(scrW, scrH, "tetris");

	double next_tick = 0;
	double next_frame = 0;

	init_field_state();

	// Create a new tetromino when old is locked
	create_tetromino();
	
	// Main game loop
	while (!WindowShouldClose()) {

		if (IsKeyPressed(KEY_E)) {
			edit_mode = !edit_mode;
		}
		if (edit_mode) {
			edit_map();
			if (IsKeyPressed(KEY_P)) {
				create_tetromino();
			}
		}

		input_handler();


		// should we simulate tick now?
		if (GetTime() >= next_tick) {
			next_tick = next_tick + (1.0 / ticks_per_sec);
			tick_counter++;
			tick();
		}

		t_ghost.p.x = tetromino.p.x;
		t_ghost.p.y = tetromino.p.y;
		float copy_pos_x = t_ghost.p.x;
		float copy_pos_y = t_ghost.p.y;

		while(can_place_ghost( copy_pos_x, copy_pos_y + 1)){
			t_ghost.p.y += 1;
			copy_pos_y = t_ghost.p.y;
		}

		// should we render frame now?
		if (GetTime() >= next_frame) {
			next_frame = next_frame + (1.0 / frames_per_sec);
			check_row_clear();
			render();
		}

		// find which happens sooner: tick or render
		double soonest_event = next_tick < next_frame ? next_tick : next_frame;

		// change timestamp to duration, we want to sleep for duration
		double sleep_duration = soonest_event - GetTime();

		// sleep duration can be negative or 0 - in this case it`s not allowed to sleep
		if (sleep_duration > 0.0) {
			// wait for next event
			WaitTime(sleep_duration);
		}
	}
	
	// De-Initialization
	CloseWindow();
	return 0;
}
