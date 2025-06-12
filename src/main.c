#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define SIZE 4

typedef enum {
	DIR_RIGHT,
	DIR_LEFT,
	DIR_UP,
	DIR_DOWN,
} Direction;

typedef enum {
	MOVE_NONE,
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_UP,
	MOVE_DOWN,
} MoveRequest;

static float easeOutCubic(float t) {
	return 1.0 - (1.0 - t) * (1.0 - t) * (1.0 - t);
}

static Color getColor(int tile) {
	switch (tile) {
		case 2: return ORANGE;
		case 4: return YELLOW;
		case 8: return GREEN;
		case 16: return BLUE;
		case 32: return PURPLE;
		// ...
	}
}

bool checkIfAnimating(float t[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			if (t[y][x] < 1.0) {
				return true;
			}
		}
	}
	return false;
}

static void boardCopy(const int board[SIZE][SIZE], int copy[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			copy[y][x] = board[y][x];
		}
	}
}

static int boardCmp(const int board[SIZE][SIZE], const int other[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			if (board[y][x] < other[y][x]) {
				return -1;
			}
			if (board[y][x] > other[y][x]) {
				return 1;
			}
		}
	}
	return 0;
}

static void slideBoardUp(const int board[SIZE][SIZE], int boardOut[SIZE][SIZE], int xOut[SIZE][SIZE], int yOut[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			boardOut[y][x] = board[y][x];
			xOut[y][x] = x;
			yOut[y][x] = y;
		}
	}
	for (int x = 0; x < SIZE; x++) {
		for (int b = 0; b < SIZE; b++) {
			if (boardOut[b][x] == 0) continue;
			for (int t = b - 1; t >= 0; --t) {
				if (boardOut[t][x] == 0) {
					yOut[b][x] = t;
				} else if (boardOut[t][x] == boardOut[b][x]) {
					yOut[b][x] = t;
					boardOut[b][x] *= 2;
					break;
				} else {
					break;
				}
			}
			int temp = boardOut[b][x];
			boardOut[b][x] = 0;
			boardOut[yOut[b][x]][xOut[b][x]] = temp;
		}
	}
}

static void slideBoardDown(const int board[SIZE][SIZE], int boardOut[SIZE][SIZE], int xOut[SIZE][SIZE], int yOut[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			boardOut[y][x] = board[y][x];
			xOut[y][x] = x;
			yOut[y][x] = y;
		}
	}
	for (int x = 0; x < SIZE; x++) {
		for (int t = SIZE - 1; t >= 0; --t) {
			if (boardOut[t][x] == 0) continue;
			for (int b = t + 1; b < SIZE; b++) {
				if (boardOut[b][x] == 0) {
					yOut[t][x] = b;
				} else if (boardOut[b][x] == boardOut[t][x]) {
					yOut[t][x] = b;
					boardOut[t][x] *= 2;
					break;
				} else {
					break;
				}
			}
			int temp = boardOut[t][x];
			boardOut[t][x] = 0;
			boardOut[yOut[t][x]][xOut[t][x]] = temp;
		}
	}
}


static void slideBoardRight(const int board[SIZE][SIZE], int boardOut[SIZE][SIZE], int xOut[SIZE][SIZE], int yOut[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			boardOut[y][x] = board[y][x];
			xOut[y][x] = x;
			yOut[y][x] = y;
		}
	}
	for (int y = 0; y < SIZE; y++) {
		for (int l = SIZE - 1; l >= 0; --l) {
			if (boardOut[y][l] == 0) continue;
			for (int r = l + 1; r < SIZE; r++) {
				if (boardOut[y][r] == 0) {
					xOut[y][l] = r;
				} else if (boardOut[y][r] == boardOut[y][l]) {
					xOut[y][l] = r;
					boardOut[y][l] *= 2;
					break;
				} else {
					break;
				}
			}
			int temp = boardOut[y][l];
			boardOut[y][l] = 0;
			boardOut[yOut[y][l]][xOut[y][l]] = temp;
		}
	}
}

static void slideBoardLeft(const int board[SIZE][SIZE], int boardOut[SIZE][SIZE], int xOut[SIZE][SIZE], int yOut[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			boardOut[y][x] = board[y][x];
			xOut[y][x] = x;
			yOut[y][x] = y;
		}
	}
	for (int y = 0; y < SIZE; y++) {
		for (int r = 0; r < SIZE; r++) {
			if (boardOut[y][r] == 0) continue;
			for (int l = r - 1; l >= 0; --l) {
				if (boardOut[y][l] == 0) {
					xOut[y][r] = l;
				} else if (boardOut[y][l] == boardOut[y][r]) {
					xOut[y][r] = l;
					boardOut[y][r] *= 2;
					break;
				} else {
					break;
				}
			}
			int temp = boardOut[y][r];
			boardOut[y][r] = 0;
			boardOut[yOut[y][r]][xOut[y][r]] = temp;
		}
	}
}

static void slideBoard(const int board[SIZE][SIZE], Direction dir, int boardOut[SIZE][SIZE], int xOut[SIZE][SIZE], int yOut[SIZE][SIZE]) {
	switch (dir) {
		case DIR_RIGHT: slideBoardRight(board, boardOut, xOut, yOut); break;
		case DIR_LEFT: slideBoardLeft(board, boardOut, xOut, yOut); break;
		case DIR_DOWN: slideBoardDown(board, boardOut, xOut, yOut); break;
		case DIR_UP: slideBoardUp(board, boardOut, xOut, yOut); break;
	}
}

int main(void) {

	int tilesToSpawn = 2;
	MoveRequest requestedMove = MOVE_NONE;
	bool isAnimating = false;
	bool resultPending = false;

	int board[SIZE][SIZE];
	int boardNext[SIZE][SIZE];
	int xNext[SIZE][SIZE];
	int yNext[SIZE][SIZE];
	float tAnimPos[SIZE][SIZE];
	float tAnimSize[SIZE][SIZE];
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			board[y][x] = 0;
			boardNext[y][x] = 0;
			xNext[y][x] = x;
			yNext[y][x] = y;
			tAnimPos[y][x] = 0.0;
			tAnimSize[y][x] = 0.0;
		}
	}

	int screenWidth = 512;
	int screenHeight = 512;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "2048");

	Color backgroundColor = { 128, 128, 128, 255 };
	float animSizeSpeed = 4.0;
	float animPosSpeed = 4.0;

	while (!WindowShouldClose()) {

		while (tilesToSpawn > 0) {
			int x, y;
			do {
				x = GetRandomValue(0, SIZE - 1);
				y = GetRandomValue(0, SIZE - 1);
			} while (board[y][x] != 0);
			board[y][x] = GetRandomValue(1, 10) < 10 ? 2 : 4;
			tAnimSize[y][x] = 0.0;
			tilesToSpawn -= 1;
		}

		if (requestedMove == MOVE_NONE) {
			int key = GetKeyPressed();
			if (key == KEY_RIGHT) requestedMove = MOVE_RIGHT;
			if (key == KEY_LEFT) requestedMove = MOVE_LEFT;
			if (key == KEY_DOWN) requestedMove = MOVE_DOWN;
			if (key == KEY_UP) requestedMove = MOVE_UP;
		}

		if (requestedMove != MOVE_NONE) {
			if (resultPending) {
				for (int y = 0; y < SIZE; y++) {
					for (int x = 0; x < SIZE; x++) {
						tAnimPos[y][x] = 1.0;
						tAnimSize[y][x] = 1.0;
					}
				}
			} else {
				switch (requestedMove) {
					case MOVE_NONE: break; // unreachable
					case MOVE_RIGHT: slideBoard(board, DIR_RIGHT, boardNext, xNext, yNext); break;
					case MOVE_LEFT: slideBoard(board, DIR_LEFT, boardNext, xNext, yNext); break;
					case MOVE_UP: slideBoard(board, DIR_UP, boardNext, xNext, yNext); break;
					case MOVE_DOWN: slideBoard(board, DIR_DOWN, boardNext, xNext, yNext); break;
				}
				if (boardCmp(board, boardNext) != 0) {
					for (int y = 0; y < SIZE; y++) {
						for (int x = 0; x < SIZE; x++) {
							tAnimPos[y][x] = 0.0;
							// tAnimSize[y][x] = 0.0;
						}
					}
					resultPending = true;
				}
				requestedMove = MOVE_NONE;
			}
		}

		isAnimating = checkIfAnimating(tAnimPos) || checkIfAnimating(tAnimSize);
		// isAnimating = false;

		if (!isAnimating && resultPending) {
			for (int y = 0; y < SIZE; y++) {
				for (int x = 0; x < SIZE; x++) {
					board[y][x] = boardNext[y][x];
					xNext[y][x] = x;
					yNext[y][x] = y;
				}
			}
			tilesToSpawn += 1;
			resultPending = false;
		}

		float deltaTime = GetFrameTime();

		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();

		for (int y = 0; y < SIZE; y++) {
			for (int x = 0; x < SIZE; x++) {
				tAnimSize[y][x] = Clamp(tAnimSize[y][x] + animSizeSpeed * deltaTime, 0.0, 1.0);
				tAnimPos[y][x] = Clamp(tAnimPos[y][x] + animPosSpeed * deltaTime, 0.0, 1.0);
			}
		}

		BeginDrawing();
		ClearBackground(backgroundColor);

		float tileWidth = (float)screenWidth / (float)SIZE;
		float tileHeight = (float)screenHeight / (float)SIZE;

		Vector2 tileSize = { tileWidth, tileHeight };

		for (int y = 0; y < SIZE; y++) {
			for (int x = 0; x < SIZE; x++) {
				if (board[y][x] == 0) continue;
				Vector2 start = { x, y };
				Vector2 end = { xNext[y][x], yNext[y][x] };
				Vector2 pos = Vector2Multiply(Vector2Lerp(start, end, easeOutCubic(tAnimPos[y][x])), tileSize);
				// Vector2 pos = Vector2Multiply(end, tileSize);
				Vector2 size = Vector2Scale(Vector2Scale(tileSize, 0.9), easeOutCubic(tAnimSize[y][x]));
				Color color = getColor(board[y][x]);
				DrawRectangleV(pos, size, color);
				DrawText(TextFormat("%d", board[y][x]), pos.x, pos.y, size.y, WHITE);
			}
		}
		EndDrawing();
	}

	CloseWindow();

	return 0;
}
