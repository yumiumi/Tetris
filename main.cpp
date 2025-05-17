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
	int rotation = 0;
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

void create_tetromino() {
	tetromino.type = TETROMINO_L; 
	tetromino.p = { 0,0 };
	tetromino.rotation = 0;
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

			Vector2 scale_tile = tile_to_px({ float(x), float(y) });
			Vector2 piece_pos = Vector2Add(t.p, scale_tile);

			if (tetrominoShapes[t.type][y][x] == 1) {
				DrawRectangleV(centerize(piece_pos), t_size, YELLOW);
			}
		}
	}
}

void render() {
	BeginDrawing();
		ClearBackground(BLACK);
			render_map();
			render_tetromino(tetromino);
	EndDrawing();
}

void input_handler() {
	if (IsKeyPressed(KEY_RIGHT)) {
		tetromino.p.x += tile;
	}
	if (IsKeyPressed(KEY_LEFT)) {
		tetromino.p.x -= tile;
	}
	if (IsKeyPressed(KEY_DOWN)) {
		tetromino.p.y += tile;
	}
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
