#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define SIZE 4

typedef enum {
	TILE_EMPTY,
	TILE_2,
	TILE_4,
	TILE_8,
	TILE_16,
	TILE_32,
	TILE_64,
	TILE_128,
	TILE_256,
	TILE_512,
	TILE_1024,
	TILE_2048
} TileValue;

typedef struct {
	TileValue value;
	int xsrc, ysrc;
	float tspawn, tslide;
} Tile;

static float easeOutCubic(float t) {
	return 1.0 - (1.0 - t) * (1.0 - t) * (1.0 - t);
}

static Color getColor(TileValue val) {
	switch (val) {
		case TILE_2:      return ColorFromHSV(225.0, 0.4, 0.3);   // dusty blue
		case TILE_4:      return ColorFromHSV(210.0, 0.45, 0.32); // steely blue
		case TILE_8:      return ColorFromHSV(200.0, 0.5, 0.35);  // dark cyan-blue
		case TILE_16:     return ColorFromHSV(185.0, 0.5, 0.33);  // cold teal
		case TILE_32:     return ColorFromHSV(170.0, 0.5, 0.31);  // murky aquamarine
		case TILE_64:     return ColorFromHSV(255.0, 0.45, 0.36); // deep indigo
		case TILE_128:    return ColorFromHSV(270.0, 0.5, 0.38);  // moody violet
		case TILE_256:    return ColorFromHSV(285.0, 0.5, 0.4);   // soft purple
		case TILE_512:    return ColorFromHSV(300.0, 0.5, 0.38);  // orchid
		case TILE_1024:   return ColorFromHSV(315.0, 0.45, 0.35); // pink-magenta
		case TILE_2048:   return ColorFromHSV(240.0, 0.5, 0.42);  // dark royal blue
		case TILE_EMPTY:  exit(1);
	}
}

static void slideLeft(Tile board[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		int left = 0;
		for (int x = 0; x < SIZE; x++) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[y][left].value = board[y][x].value;
			board[y][left].xsrc = x;
			left++;
		}
		for (int x = left; x < SIZE; x++) {
			board[y][x].value = TILE_EMPTY;
		}
		for (int x = 0; x < left - 1; x++) {
			if (board[y][x].value == board[y][x + 1].value) {
				board[y][x].value++;
				board[y][x + 1].value = TILE_EMPTY;
				board[y][x].xsrc = board[y][x + 1].xsrc;
				x++;
			}
		}
		int final = 0;
		for (int x = 0; x < left; x++) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[y][final].value = board[y][x].value;
			board[y][final].xsrc = board[y][x].xsrc;
			final++;
		}
		for (int x = final; x < SIZE; x++) {
			board[y][x].value = TILE_EMPTY;
		}
	}
}

static void slideRight(Tile board[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		int right = SIZE - 1;
		for (int x = SIZE - 1; x >= 0; --x) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[y][right].value = board[y][x].value;
			board[y][right].xsrc = x;
			--right;
		}
		for (int x = right; x >= 0; --x) {
			board[y][x].value = TILE_EMPTY;
		}
		for (int x = SIZE - 1; x > right + 1; --x) {
			if (board[y][x].value == board[y][x - 1].value) {
				board[y][x].value++;
				board[y][x - 1].value = TILE_EMPTY;
				board[y][x].xsrc = board[y][x - 1].xsrc;
				x++;
			}
		}
		int final = SIZE - 1;
		for (int x = SIZE - 1; x > right; --x) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[y][final].value = board[y][x].value;
			board[y][final].xsrc = board[y][x].xsrc;
			--final;
		}
		for (int x = final; x >= 0; --x) {
			board[y][x].value = TILE_EMPTY;
		}
	}
}

static void slideUp(Tile board[SIZE][SIZE]) {
	for (int x = 0; x < SIZE; x++) {
		int top = 0;
		for (int y = 0; y < SIZE; y++) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[top][x].value = board[y][x].value;
			board[top][x].ysrc = y;
			top++;
		}
		for (int y = top; y < SIZE; y++) {
			board[y][x].value = TILE_EMPTY;
		}
		for (int y = 0; y < top - 1; y++) {
			if (board[y][x].value == board[y + 1][x].value) {
				board[y][x].value++;
				board[y + 1][x].value = TILE_EMPTY;
				board[y][x].ysrc = board[y + 1][x].ysrc;
				y++;
			}
		}
		int final = 0;
		for (int y = 0; y < top; y++) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[final][x].value = board[y][x].value;
			board[final][x].ysrc = board[y][x].ysrc;
			final++;
		}
		for (int y = final; y < SIZE; y++) {
			board[y][x].value = TILE_EMPTY;
		}
	}
}

static void slideDown(Tile board[SIZE][SIZE]) {
	for (int x = 0; x < SIZE; x++) {
		int bottom = SIZE - 1;
		for (int y = SIZE - 1; y >= 0; --y) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[bottom][x].value = board[y][x].value;
			board[bottom][x].ysrc = y;
			--bottom;
		}
		for (int y = bottom; y >= 0; --y) {
			board[y][x].value = TILE_EMPTY;
		}
		for (int y = SIZE - 1; y > bottom + 1; --y) {
			if (board[y][x].value == board[y - 1][x].value) {
				board[y][x].value++;
				board[y - 1][x].value = TILE_EMPTY;
				board[y][x].ysrc = board[y - 1][x].ysrc;
				y++;
			}
		}
		int final = SIZE - 1;
		for (int y = SIZE - 1; y > bottom; --y) {
			if (board[y][x].value == TILE_EMPTY) continue;
			board[final][x].value = board[y][x].value;
			board[final][x].ysrc = board[y][x].ysrc;
			--final;
		}
		for (int y = final; y >= 0; --y) {
			board[y][x].value = TILE_EMPTY;
		}
	}
}

static bool anyMoved(Tile board[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			if (board[y][x].value == TILE_EMPTY) continue;
			if (board[y][x].xsrc != x || board[y][x].ysrc != y) {
				return true;
			}
		}
	}
	return false;
}

static void boardCopy(const Tile board[SIZE][SIZE], Tile copy[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			copy[y][x] = board[y][x];
		}
	}
}

static void boardCommit(Tile board[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			board[y][x].xsrc = x;
			board[y][x].ysrc = y;
			board[y][x].tslide = 1.0;
			board[y][x].tspawn = 1.0;
		}
	}
}

static bool isWon(const Tile board[SIZE][SIZE]) {
	for (int y = 0; y < SIZE; y++) {
		for (int x = 0; x < SIZE; x++) {
			if (board[y][x].value == TILE_2048) {
				return true;
			}
		}
	}
	return false;
}

static bool isLost(const Tile board[SIZE][SIZE]) {

	Tile result[SIZE][SIZE];

	boardCopy(board, result);
	boardCommit(result);
	slideLeft(result);
	if (anyMoved(result)) return false;

	boardCopy(board, result);
	boardCommit(result);
	slideRight(result);
	if (anyMoved(result)) return false;

	boardCopy(board, result);
	boardCommit(result);
	slideUp(result);
	if (anyMoved(result)) return false;

	boardCopy(board, result);
	boardCommit(result);
	slideDown(result);
	if (anyMoved(result)) return false;

	return true;
}

int main(void) {

	Tile board[SIZE][SIZE];
	int tilesToSpawn;
	bool won;
	bool lost;


	int screenWidth = 512;
	int screenHeight = 512;

	InitAudioDevice();

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "2048");
	SetWindowMinSize(256, 256);

	Font font = LoadFontEx("assets/font.ttf", 128, NULL, 0);
	Sound slideSound = LoadSound("assets/pop.wav");
	Sound stuckSound = LoadSound("assets/stuck.wav");
	Sound winSound = LoadSound("assets/win.wav");
	Sound loseSound = LoadSound("assets/lose.wav");
	Sound restartSound = LoadSound("assets/restart.wav");
	Music music = LoadMusicStream("assets/music.mp3");
	PlayMusicStream(music);
	SetMusicVolume(music, 0.2);
	music.looping = true;

	Color backgroundColor = ColorFromHSV(240.0, 0.4, 0.2);
	float slidespeed = 4.0;
	float spawnspeed = 4.0;

	Vector2 dragStartPos;
	Vector2 dragEndPos;
	bool draggingMouse = false;
	int dragPreviewDir = KEY_NULL;

	bool reset = true;

	while (!WindowShouldClose()) {

		UpdateMusicStream(music);

		float dt = GetFrameTime();

		if (reset) {
			reset = false;
			won = false;
			lost = false;
			for (int y = 0; y < SIZE; y++) {
				for (int x = 0; x < SIZE; x++) {
					board[y][x].value = TILE_EMPTY;
					board[y][x].xsrc = x;
					board[y][x].ysrc = y;
					board[y][x].tspawn = 0.0;
					board[y][x].tslide = 1.0;
				}
			}
			tilesToSpawn = 2;
		}

		while (tilesToSpawn > 0) {
			--tilesToSpawn;
			int x, y;
			do {
				x = GetRandomValue(0, SIZE - 1);
				y = GetRandomValue(0, SIZE - 1);
			} while (board[y][x].value != TILE_EMPTY);
			board[y][x].value = GetRandomValue(1, 8) == 8 ? TILE_4 : TILE_2;
			board[y][x].xsrc = x;
			board[y][x].ysrc = y;
			board[y][x].tspawn = 0.0;
			board[y][x].tslide = 1.0;
		}

		if (!won && !lost) {
			if (isWon(board)) {
				PlaySound(winSound);
				won = true;
			} else if (isLost(board)) {
				PlaySound(loseSound);
				lost = true;
			}
		}

		if (!won && !lost) {

			int dragDir = KEY_NULL;

			if (draggingMouse) {
				if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
					draggingMouse = false;
				}
				dragEndPos = GetMousePosition();
				Vector2 dragDelta = Vector2Subtract(dragEndPos, dragStartPos);
				// Vector2 dragDeltaScaled = Vector2Divide(dragDelta, (Vector2) { screenWidth, screenHeight });
				float x = dragDelta.x;
				float y = dragDelta.y;
				float xmag = fabsf(x);
				float ymag = fabsf(y);
				dragPreviewDir = KEY_NULL;
				float threshold = 32.0;
				if (xmag > threshold || ymag > threshold) {
					if (xmag > ymag) {
						if (x < 0.0) {
							dragPreviewDir = KEY_LEFT;
						} else {
							dragPreviewDir = KEY_RIGHT;
						}
					} else {
						if (y < 0.0) {
							dragPreviewDir = KEY_UP;
						} else {
							dragPreviewDir = KEY_DOWN;
						}
					}
				}
				if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
					dragDir = dragPreviewDir;
					draggingMouse = false;
				}
			} else {
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					dragStartPos = GetMousePosition();
					dragPreviewDir = KEY_NULL;
					draggingMouse = true;
				}
			}

			int key = GetKeyPressed();

			if (key != KEY_NULL || dragDir != KEY_NULL) {

				boardCommit(board);

				bool hasResult = false;
				Tile result[SIZE][SIZE];
				boardCopy(board, result);
				if (key == KEY_LEFT || key == KEY_A || dragDir == KEY_LEFT) {
					slideLeft(result);
					hasResult = true;
				} else if (key == KEY_RIGHT || key == KEY_D || dragDir == KEY_RIGHT) {
					slideRight(result);
					hasResult = true;
				} else if (key == KEY_UP || key == KEY_W || dragDir == KEY_UP) {
					slideUp(result);
					hasResult = true;
				} else if (key == KEY_DOWN || key == KEY_S || dragDir == KEY_DOWN) {
					slideDown(result);
					hasResult = true;
				}
				if (hasResult) {
					if (anyMoved(result)) {
						boardCopy(result, board);
						for (int y = 0; y < SIZE; y++) {
							for (int x = 0; x < SIZE; x++) {
								board[y][x].tslide = 0.0;
							}
						}
						SetSoundPitch(slideSound, 1.0 + 0.2 * (2.0 * 0.01 * GetRandomValue(1, 100) - 1.0));
						SetSoundVolume(slideSound, 1.0 - 0.1 * (0.01 * GetRandomValue(1, 100)));
						PlaySound(slideSound);
						tilesToSpawn++;
					} else {
						SetSoundPitch(stuckSound, 1.0 + 0.2 * (2.0 * 0.01 * GetRandomValue(1, 100) - 1.0));
						SetSoundVolume(stuckSound, 1.0 - 0.1 * (0.01 * GetRandomValue(1, 100)));
						PlaySound(stuckSound);
					}
				}
			}
		}

		if (IsKeyPressed(KEY_R)) {
			reset = true;
			SetSoundPitch(restartSound, 1.0 + 0.2 * (2.0 * 0.01 * GetRandomValue(1, 100) - 1.0));
			SetSoundVolume(restartSound, 1.0 - 0.1 * (0.01 * GetRandomValue(1, 100)));
			PlaySound(restartSound);
		}

		for (int y = 0; y < SIZE; y++) {
			for (int x = 0; x < SIZE; x++) {
				board[y][x].tslide = Clamp(board[y][x].tslide + slidespeed * dt, 0.0, 1.0);
				board[y][x].tspawn = Clamp(board[y][x].tspawn + spawnspeed * dt, 0.0, 1.0);
			}
		}

		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();

		BeginDrawing();
		ClearBackground(backgroundColor);

		float tileWidth = (float)screenWidth / (float)SIZE;
		float tileHeight = (float)screenHeight / (float)SIZE;

		for (int y = 0; y < SIZE; y++) {
			for (int x = 0; x < SIZE; x++) {
				TileValue value = board[y][x].value;
				if (value == TILE_EMPTY) continue;
				float scale = easeOutCubic(board[y][x].tspawn);
				Vector2 tileSize = {
					tileWidth * scale,
					tileHeight * scale
				};
				Vector2 tileOffset = {
					0.5 * (tileWidth - tileSize.x),
					0.5 * (tileHeight - tileSize.y)
				};
				Vector2 srcPos = { board[y][x].xsrc * tileWidth, board[y][x].ysrc * tileHeight };
				Vector2 dstPos = { x * tileWidth, y * tileHeight };
				Vector2 tilePos = Vector2Add(tileOffset, Vector2Lerp(srcPos, dstPos, easeOutCubic(board[y][x].tslide)));
				Color tileColor = getColor(value);
				Rectangle tileRect = {
					.x = tilePos.x,
					.y = tilePos.y,
					.width = tileSize.x,
					.height = tileSize.y
				};
				const char* text = TextFormat("%d", 1 << value);
				float fontSize = fminf(tileSize.x, tileSize.y) * 0.36;
				Vector2 textSize = MeasureTextEx(font, text, fontSize, 0.0);
				Vector2 textOffset = {
					0.5 * (tileSize.x - textSize.x),
					0.5 * (tileSize.y - textSize.y)
				};
				Vector2 textPos = {
					tilePos.x + textOffset.x,
					tilePos.y + textOffset.y,
				};
				Color textColor = WHITE;
				DrawRectangleRounded(tileRect, 0.4, 8, tileColor);
				DrawTextEx(font, text, textPos, fontSize, 0.0, textColor);
			}
		}

		if (draggingMouse) {
			// if (dragPreviewDir != KEY_NULL) {
			// 	Vector2 dragDelta = Vector2Subtract(dragEndPos, dragStartPos);
			// 	float a = atan2f(dragDelta.y, dragDelta.x);
			// 	DrawCircleSector(dragStartPos, 128.0, a * RAD2DEG - 120.0, a * RAD2DEG + 120.0, 12, ColorAlpha(WHITE, 0.1));
			// }
			// DrawCircleGradient(dragStartPos.x, dragStartPos.y, 16.0, ColorAlpha(WHITE, 0.05), ColorAlpha(WHITE, 0.0));
			// DrawCircleGradient(dragEndPos.x, dragEndPos.y, 16.0, ColorAlpha(WHITE, 0.05), ColorAlpha(WHITE, 0.0));
			if (dragPreviewDir == KEY_LEFT) {
				DrawRectangleGradientH(0, 0, screenWidth / 2, screenHeight, ColorAlpha(WHITE, 0.1), ColorAlpha(WHITE, 0.0));
			}
			if (dragPreviewDir == KEY_RIGHT) {
				DrawRectangleGradientH(screenWidth / 2, 0, screenWidth, screenHeight, ColorAlpha(WHITE, 0.0), ColorAlpha(WHITE, 0.1));
			}
			if (dragPreviewDir == KEY_UP) {
				DrawRectangleGradientV(0, 0, screenWidth, screenHeight / 2, ColorAlpha(WHITE, 0.1), ColorAlpha(WHITE, 0.0));
			}
			if (dragPreviewDir == KEY_DOWN) {
				DrawRectangleGradientV(0, screenHeight / 2, screenWidth, screenHeight, ColorAlpha(WHITE, 0.0), ColorAlpha(WHITE, 0.1));
			}
		}

		if (won || lost) {
			const char* text;
			if (won) {
				text = "You won! :)\nPress R to play again";
			}
			if (lost) {
				text = "You lost... :(\nPress R to try again";
			}
			float fontSize = fminf(screenWidth, screenHeight) * 0.084;
			Vector2 textSize = MeasureTextEx(font, text, fontSize, 0.0);
			Vector2 textPos = {
				0.5 * screenWidth - 0.5 * textSize.x,
				0.5 * screenHeight - 0.5 * textSize.y,
			};
			Color textColor = WHITE;
			DrawTextEx(font, text, textPos, fontSize, 0.0, textColor);
		}

		// DrawFPS(4, 4);
		EndDrawing();
	}

	StopMusicStream(music);

	UnloadMusicStream(music);
	UnloadFont(font);
	UnloadSound(slideSound);
	UnloadSound(stuckSound);
	UnloadSound(winSound);
	UnloadSound(loseSound);
	UnloadSound(restartSound);

	CloseAudioDevice();
	CloseWindow();

	return 0;
}
