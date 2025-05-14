#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cassert>
#include <intrin.h>
#include "tetromino.hpp"

// to do:
// tetrominos 
// randomly choose one tetromino, place it in the top-middle position of the field
// user input
// tetromino falls - ticks 
// rotation
// collision
// check if tetromino fits
// clear a line when it's full
// score

using namespace std;

const int scrW = 700;
const int scrH = 1000;

// width and height of playing field
const int fW = 12;
const int fH = 21;

const int t_size = 26;

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
	int px = (fW / 2) - 1;
	int py = 0;
	int rotation = 0;
};

Tetromino piece;

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

// Copy given tetromino type to the map
void piece_to_map(TetrominoType type) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (tetrominoShapes[type][y][x] == 1) {
				field[{(x + fW/2) - 1, y}].state = HAS_VALUE;
			}
		}
	}
}

// function that moves position of given tile to the center of the screen
Vector2 offset_map(Vector2 tile_pos) {
	float w_off = scrW/2 - fW/2 * t_size;
	float h_off = scrH/2 - fH/2 * t_size;
	return { tile_pos.x + w_off, tile_pos.y + h_off };
}

// Check tile's state to render it
// and offset playing field to the center of the screen while rendering
void render_tile() {
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {
			Vector2 rec_pos = { x * t_size, y * t_size };
			Vector2 rec_size = { t_size, t_size };
			if (field[{x, y}].state == EMPTY){
				DrawRectangleV(offset_map(rec_pos), rec_size, BLACK);
			}
			if (field[{x, y}].state == BORDER) {
				DrawRectangleV(offset_map(rec_pos), rec_size, DARKGRAY);
			}
			if (field[{x, y}].state == HAS_VALUE) {
				DrawRectangleV(offset_map(rec_pos), rec_size, YELLOW);
			}
		}
	}
}

void render() {
	BeginDrawing();
		ClearBackground(BLACK);
			render_tile();
	EndDrawing();
}

int main() {
	//Initialization
	InitWindow(scrW, scrH, "tetris");

	init_field_state();
	
	SetTargetFPS(60);
	
	// Main game loop
	while (!WindowShouldClose()) {
		// Game timing

		// Input

		// Game logic
		piece_to_map(TETROMINO_Z);

		// Update
		render();
	}
	
	// De-Initialization
	CloseWindow();
	return 0;
}
