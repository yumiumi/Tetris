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

const int t_size = 20;

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

// The state of each tile on the map at the start
void init_fieldState() {
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

// function that moves position of given tile to the center of the screen
Vector2 offset_map(Vector2 tile_pos) {
	float w_off = scrW/2 - fW/2 * t_size;
	float h_off = scrH/2 - fH/2 * t_size;
	return { tile_pos.x + w_off, tile_pos.y + h_off };
}

// Check tile's state to render it
// and offset playing field to the center of the screen while rendering
void render_field() {
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {
			Vector2 rec_pos = { x * t_size, y * t_size };
			Vector2 rec_size = { t_size, t_size };
			if (field[{x, y}].state == BORDER) {
				DrawRectangleV(offset_map(rec_pos), rec_size, WHITE);
			}
			else {
				DrawRectangleV(offset_map(rec_pos), rec_size, BLACK);
			}
		}
	}
}

void render() {
	BeginDrawing();
		ClearBackground(BLACK);
			render_field();
	EndDrawing();
}

int main() {
	//Initialization
	InitWindow(scrW, scrH, "tetris");

	init_fieldState();
	
	SetTargetFPS(60);
	
	// Main game loop
	while (!WindowShouldClose()) {
		// Update
		render();
	}
	
	// De-Initialization
	CloseWindow();
	return 0;
}
