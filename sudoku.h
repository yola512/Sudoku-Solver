#ifndef SUDOKU_H
#define SUDOKU_H

#include "utils.h"
#include "string.h"

bool unUsedInRow(int **board, int i, int number);
bool unUsedInColumn(int **board, int j, int number);
bool unUsedInBox(int **board, int i, int j, int number);
bool finalCheck(int **board, int i, int j, int number);
void fillBox(int **board, int row, int col);
void fillDiagonal(int **board);
bool fillRemaining(int **board, int i, int j);
void removeXDigits(int **board, int x);
int getNumberOfCellsToRemove(Difficulty difficulty);
int **generateSudoku(Difficulty difficulty, Game *game);

#endif //SUDOKU_H
