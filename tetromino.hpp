#pragma once 

enum TetrominoType {
	TETROMINO_I,
	TETROMINO_T,
	TETROMINO_O,
	TETROMINO_L,
	TETROMINO_J,
	TETROMINO_S,
	TETROMINO_Z,
};

int tetrominoShapes[7][4][4] = {
	{ // TETROMINO_I = 0
		{0, 0, 0, 0},
		{1, 1, 1, 1},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ // TETROMINO_T = 1
		{0, 1, 0, 0},
		{1, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_O = 2
		{0, 1, 1, 0},
		{0, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_L = 3
		{0, 0, 1, 0},
		{1, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_J = 4
		{1, 0, 0, 0},
		{1, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_S = 5
		{0, 1, 1, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_Z = 6
		{1, 1, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	}
};

Color TBLUE = Color{ 106, 144, 204, 255 };
Color TPINK = Color{ 204, 106, 191, 255 };
Color TYELLOW = Color{ 207, 181, 68, 255 };
Color TORANGE = Color{ 209, 124, 59, 255 };
Color TVIOLET = Color{ 112, 95, 184, 255 };
Color TLIME = Color{ 151, 181, 91, 255 };
Color TRED = Color{ 207, 64, 64, 255 };

Color GRID_GRAY = Color{ 38, 38, 38, 255 };

Color GHOST_GRAY = Color{ 60, 60, 60, 255/2 };

Color colors[7] = { TBLUE, TPINK, TYELLOW, TORANGE, TVIOLET, TLIME, TRED };
