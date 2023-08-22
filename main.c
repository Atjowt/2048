#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define GOAL 2048
#define WIDTH 4
#define HEIGHT 4
#define H_PADDING 5
#define V_PADDING 2

typedef int32_t i32;

i32 board[HEIGHT][WIDTH] = { };

// Generate a random integer in the range [min, max].
i32 rand_range(i32 min, i32 max)
{
    return min + (rand() % (max - min + 1));
}

// If there exists at least one square with the target value, the game is won.
bool has_won(void)
{
    for (i32 y = 0; y < HEIGHT; y++)
    {
        for (i32 x = 0; x < WIDTH; x++)
        {
            if (board[y][x] == GOAL)
            {
                return true;
            }
        }
    }
    return false;
}

// If there exists no empty square and none of the squares can merge, then no moves are possible and the game is lost.
bool has_lost(void)
{
    for (i32 y = 0; y < HEIGHT; y++)
    {
        for (i32 x = 0; x < WIDTH ; x++)
        {
            if (board[y][x] == 0)
            {
                return false;
            }
        }
    }

    for (i32 y = 1; y < HEIGHT - 1; y++)
    {
        for (i32 x = 1; x < WIDTH - 1; x++)
        {
            if
            (
                board[y][x] == board[y][x - 1] ||
                board[y][x] == board[y][x + 1] ||
                board[y][x] == board[y - 1][x] ||
                board[y][x] == board[y + 1][x]
            )
            {
                return false;
            }
        }
    }

    return true;
}

// Spawn a 2 at a random empty position.
void spawn_square(void)
{
    i32 x, y;
    do
    {
        x = rand_range(0, WIDTH - 1);
        y = rand_range(0, HEIGHT - 1);
    }
    while (board[y][x] != 0);
    board[y][x] = 2;
}

// If at least one square can move left, it is possible to swipe left.
bool can_swipe_left(void)
{
    for (i32 y = 0; y < HEIGHT; y++)
    {
        for (i32 x0 = 1; x0 < WIDTH; x0++)
        {
            if (board[y][x0] != 0)
            {
                for (i32 x1 = x0; x1 >= 1; x1--)
                {
                    if (board[y][x1 - 1] == 0)
                    {
                        return true;
                    }
                    else if (board[y][x1 - 1] == board[y][x1])
                    {
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    return false;
}

// If at least one square can move right, it is possible to swipe right.
bool can_swipe_right(void)
{
    for (i32 y = 0; y < HEIGHT; y++)
    {
        for (i32 x0 = WIDTH - 1 - 1; x0 >= 0; x0--)
        {
            if (board[y][x0] != 0)
            {
                for (i32 x1 = x0; x1 < WIDTH - 1; x1++)
                {
                    if (board[y][x1 + 1] == 0)
                    {
                        return true;
                    }
                    else if (board[y][x1 + 1] == board[y][x1])
                    {
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    return false;
}

// If at least one square can move up, it is possible to swipe up.
bool can_swipe_up(void)
{
    for (i32 x = 0; x < WIDTH; x++)
    {
        for (i32 y0 = 1; y0 < HEIGHT; y0++)
        {
            if (board[y0][x] != 0)
            {
                for (i32 y1 = y0; y1 >= 1; y1--)
                {
                    if (board[y1 - 1][x] == 0)
                    {
                        return true;
                    }
                    else if (board[y1 - 1][x] == board[y1][x])
                    {
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    return false;
}

// If at least one square can move down, it is possible to swipe down.
bool can_swipe_down(void)
{
    for (i32 x = 0; x < WIDTH; x++)
    {
        for (i32 y0 = HEIGHT - 1 - 1; y0 >= 0; y0--)
        {
            if (board[y0][x] != 0)
            {
                for (i32 y1 = y0; y1 < HEIGHT - 1; y1++)
                {
                    if (board[y1 + 1][x] == 0)
                    {
                        return true;
                    }
                    else if (board[y1 + 1][x] == board[y1][x])
                    {
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
    return false;
}

void swipe_left(void)
{
    for (i32 y = 0; y < HEIGHT; y++)
    {
        for (i32 x0 = 1; x0 < WIDTH; x0++)
        {
            if (board[y][x0] != 0)
            {
                for (i32 x1 = x0; x1 >= 1; x1--)
                {
                    if (board[y][x1 - 1] == 0)
                    {
                        board[y][x1 - 1] = board[y][x1];
                        board[y][x1] = 0;
                    }
                    else if (board[y][x1 - 1] == board[y][x1])
                    {
                        board[y][x1 - 1] <<= 1;
                        board[y][x1] = 0;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

void swipe_right(void)
{
    for (i32 y = 0; y < HEIGHT; y++)
    {
        for (i32 x0 = WIDTH - 1 - 1; x0 >= 0; x0--)
        {
            if (board[y][x0] != 0)
            {
                for (i32 x1 = x0; x1 < WIDTH - 1; x1++)
                {
                    if (board[y][x1 + 1] == 0)
                    {
                        board[y][x1 + 1] = board[y][x1];
                        board[y][x1] = 0;
                    }
                    else if (board[y][x1 + 1] == board[y][x1])
                    {
                        board[y][x1 + 1] <<= 1;
                        board[y][x1] = 0;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

void swipe_up(void)
{
    for (i32 x = 0; x < WIDTH; x++)
    {
        for (i32 y0 = 1; y0 < HEIGHT; y0++)
        {
            if (board[y0][x] != 0)
            {
                for (i32 y1 = y0; y1 >= 1; y1--)
                {
                    if (board[y1 - 1][x] == 0)
                    {
                        board[y1 - 1][x] = board[y1][x];
                        board[y1][x] = 0;
                    }
                    else if (board[y1 - 1][x] == board[y1][x])
                    {
                        board[y1 - 1][x] <<= 1;
                        board[y1][x] = 0;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

void swipe_down(void)
{
    for (i32 x = 0; x < WIDTH; x++)
    {
        for (i32 y0 = HEIGHT - 1 - 1; y0 >= 0; y0--)
        {
            if (board[y0][x] != 0)
            {
                for (i32 y1 = y0; y1 < HEIGHT - 1; y1++)
                {
                    if (board[y1 + 1][x] == 0)
                    {
                        board[y1 + 1][x] = board[y1][x];
                        board[y1][x] = 0;
                    }
                    else if (board[y1 + 1][x] == board[y1][x])
                    {
                        board[y1 + 1][x] <<= 1;
                        board[y1][x] = 0;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

void clear_screen(void)
{
    printf("\x1b[H");
    printf("\x1b[2J");
}

void print_board(void)
{
    for (i32 y = 0; y < HEIGHT; y++)
    {
        for (i32 x = 0; x < WIDTH; x++)
        {
            i32 w = 0;

            if (board[y][x] == 0)
            {
                w = printf(".");
            }
            else
            {
                w = printf("%u", board[y][x]);
            }

            for (i32 _ = 0; _ < H_PADDING - w; _++)
            {
                printf(" ");
            }
        }

        for (i32 _ = 0; _ < V_PADDING; _++)
        {
            printf("\n");
        }
    }
}

int main(void)
{
    srand(time(NULL));

    spawn_square();
    spawn_square();

    while (true)
    {
        clear_screen();
        print_board();

        if (has_won())
        {
            printf("You win!\n");
            break;
        }

        if (has_lost())
        {
            printf("Game over!\n");
            break;
        }

        char c;
        scanf("%c", &c);
        fflush(stdin);

        if (c == 'w' && can_swipe_up()) swipe_up();
        else if (c == 'a' && can_swipe_left()) swipe_left();
        else if (c == 's' && can_swipe_down()) swipe_down();
        else if (c == 'd' && can_swipe_right()) swipe_right();
        else continue;

        spawn_square();
    }

    return EXIT_SUCCESS;
}