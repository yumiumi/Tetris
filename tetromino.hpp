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
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 0, 0},
	},
	{ // TETROMINO_T = 1
		{1, 1, 1, 0},
		{0, 1, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_O = 2
		{1, 1, 0, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_L = 3
		{1, 0, 0, 0},
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{0, 0, 0, 0},
	},
	{ //TETROMINO_J = 4
		{0, 1, 0, 0},
		{0, 1, 0, 0},
		{1, 1, 0, 0},
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
