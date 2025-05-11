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
// render
// field 10x20 + borders
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

const int t_size = 10;

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

struct Map {
private:
	Tile map[fH][fW];
public:
	Tile& operator[](Vec2Int v) {
		return map[v.y][v.x];
	}
};

Map field;

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

void render_field() {
	for (int y = 0; y < fH; y++) {
		for (int x = 0; x < fW; x++) {
			if (field[{x, y}].state == BORDER) {
				DrawRectangle(x * t_size, y * t_size, t_size, t_size, WHITE);
			}
			else {
				DrawRectangle(x * t_size, y * t_size, t_size, t_size, BLACK);
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
