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
const int fW = 12;
const int fH = 21;

// Tile size
const int tile = 26;

struct Vec2Int {
	int x;
	int y;
};

enum TileState {
	EMPTY,
	HAS_VALUE,
	BORDER,
};

struct Tile {
	TileState state;
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
	// Tetromino type (I, O, T, J, L, S, Z)
	TetrominoType type;
	Vector2 p = { 0,0 };
	int local_template[4][4];
};

Tetromino tetromino;

// The state of each tile on the map at the start
void init_field_state() {
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {
			if (x == 0 || x == fW - 1 || y == fH - 1) {
				field[{x, y}].state = BORDER;
			}
			else {
				field[{x,y}].state = EMPTY;
			}
		}
	}
}

TetrominoType rand_type() {
	return TetrominoType(GetRandomValue(0,6));
}

void create_tetromino() {
	tetromino.type = rand_type(); 
	tetromino.p = { 4,0 };
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			tetromino.local_template[y][x] = tetrominoShapes[tetromino.type][y][x];
		}
	}
}

// Rotates by 90 
void rotate(Tetromino* t) {
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
		{ 2,1 },
		{ 1,1 },
		{ 1,1 },
	};

	Vector2 center = centers[t->type];

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (local_tpl_copy[y][x] == 1) {
				Vector2 cell = { float(x), float(y) };
				Vector2 rel_center = Vector2Subtract(cell, center);
				Vector2 rotated_v = Vector2Rotate(rel_center, 90.f * DEG2RAD);
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
			if (pos_x + x >= 0 && pos_x + x <= fW) {
				if (pos_y + y >= 0 && pos_y + y <= fH) {
					int tile_st = int(field[{pos_x + x, pos_y + y}].state);
					if (tile_st != 0) {
						return false;
					}
				}
			}
		}
	}
	return true;
}

void input_handler() {
	int copy_pos_x = 0;
	int copy_pos_y = 0;

	if (IsKeyPressed(KEY_RIGHT)) {
		copy_pos_x = tetromino.p.x + 1;
		copy_pos_y = tetromino.p.y;
		if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
			tetromino.p.x += 1;
			cout << tetromino.p.x << ", " << tetromino.p.y << endl;
		}
	}
	if (IsKeyPressed(KEY_LEFT)) {
		copy_pos_x = tetromino.p.x - 1;
		copy_pos_y = tetromino.p.y;
		if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
			tetromino.p.x -= 1;
			cout << tetromino.p.x << ", " << tetromino.p.y << endl;
		}
	}
	if (IsKeyPressed(KEY_DOWN)) {
		copy_pos_x = tetromino.p.x;
		copy_pos_y = tetromino.p.y + 1;
		if (can_place(tetromino, copy_pos_x, copy_pos_y)) {
			tetromino.p.y += 1;
			cout << tetromino.p.x << ", " << tetromino.p.y << endl;
		}
	}
	if (IsKeyPressed(KEY_UP)) {
		Tetromino t_copy = tetromino;
		copy_pos_x = t_copy.p.x;
		copy_pos_y = t_copy.p.y;
		rotate(&t_copy);
		if (can_place(t_copy, copy_pos_x, copy_pos_y)) {
			rotate(&tetromino);
		}
		else if (can_place(t_copy, copy_pos_x + 1, copy_pos_y)) {
			tetromino.p.x += 1;
			rotate(&tetromino);
		}
		else if (can_place(t_copy, copy_pos_x - 1, copy_pos_y)) {
			tetromino.p.x -= 1;
			rotate(&tetromino);
		}
		else if (can_place(t_copy, copy_pos_x, copy_pos_y - 1)) {
			tetromino.p.y -= 1; 
			rotate(&tetromino);
		}
		else if (can_place(t_copy, copy_pos_x + 1, copy_pos_y - 1)) {
			tetromino.p.x += 1;
			tetromino.p.y -= 1;
			rotate(&tetromino);
		}
		else if (can_place(t_copy, copy_pos_x - 1, copy_pos_y - 1)) {
			tetromino.p.x -= 1;
			tetromino.p.y -= 1;
			rotate(&tetromino);
		}
		else if (can_place(t_copy, copy_pos_x + 2, copy_pos_y - 2)) {
			tetromino.p.x += 2;
			tetromino.p.y -= 2;
			rotate(&tetromino);
		}
		else if (can_place(t_copy, copy_pos_x - 2, copy_pos_y - 2)) {
			tetromino.p.x -= 2;
			tetromino.p.y -= 2;
			rotate(&tetromino);
		}
	}
}

// Copy given tetromino type to the map
void tetromino_to_map(TetrominoType type) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (tetrominoShapes[type][y][x] == 1) {
				field[{(x + fW/2) - 1, y}].state = HAS_VALUE;
			}
		}
	}
}

Vector2 tile_to_px(Vector2 v) {
	return { v.x * tile, v.y * tile };
}

// Function that moves position of given tile to the center of the screen
Vector2 centerize(Vector2 tile_pos) {
	float w = scrW/2 - fW/2 * tile;
	float h = scrH/2 - fH/2 * tile;
	return { tile_pos.x + w , tile_pos.y + h };
}

// Check tile's state to render it
// and offset playing field to the center of the screen while rendering
void render_map() {
	Vector2 t_size = { tile, tile };
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {

			Vector2 scale_tile = tile_to_px({ float(x), float(y) });

			if (field[{x, y}].state == EMPTY){
				DrawRectangleV(centerize(scale_tile), t_size, BLACK);
			}
			if (field[{x, y}].state == BORDER) {
				DrawRectangleV(centerize(scale_tile), t_size, DARKGRAY);
			}
			/*if (field[{x, y}].state == HAS_VALUE) {
				DrawRectangleV(centerize(px_pos), rec_size, GREEN)
			}*/
		}
	}
}

void render_tetromino(Tetromino const& t) {
	Vector2 t_size = { tile, tile };
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (tetromino.local_template[y][x] == 1) {
				Vector2 scale_tile = tile_to_px({ float(x), float(y) });
				Vector2 piece_pos = centerize(Vector2Add(tile_to_px(t.p), scale_tile));
				DrawRectangleV(piece_pos, t_size, YELLOW);
			}
		}
	}
}

void change_type(Tetromino* t) {
}

void render() {
	BeginDrawing();
		ClearBackground(BLACK);
			render_map();
			render_tetromino(tetromino);
	EndDrawing();
}

int main() {
	//Initialization
	InitWindow(scrW, scrH, "tetris");

	init_field_state();

	// Create a new tetromino when old is locked
	create_tetromino();
	
	SetTargetFPS(60);
	
	// Main game loop
	while (!WindowShouldClose()) {
		// Game timing

		// Input
		if (IsKeyPressed(KEY_P)) {
			create_tetromino();
		}

		input_handler();
		// Game logic
		//check collision 
		//tetromino_to_map(tetromino.type);
		// Update
		render();
	}
	
	// De-Initialization
	CloseWindow();
	return 0;
}
