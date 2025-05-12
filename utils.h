#ifndef UTILS_H
#define UTILS_H

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include <stdbool.h>

typedef enum {
    EASY,
    MEDIUM,
    HARD
  } Difficulty;

typedef struct {
    int **board;
    Difficulty difficulty;
} Game;

Game *createNewGame(Difficulty difficulty);
int **allocateBoard();
void freeGame(Game *game);
void printBoard(Game *game);
void copyBoard(int **source, int **destination);
void swap(int *a, int *b);
void fill_board_with_nums(int **board);

#endif //UTILS_H

