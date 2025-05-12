#ifndef SOLVER_H
#define SOLVER_H

#include "time.h"
#include "sudoku.h"

typedef struct {
    int **solution;
    int **frozen; // frozen cells (unchangable)
    double energy;
} GameState;

int calculateEnergy(int **board);
void swap(int *a, int *b);
int generateNeighbour(GameState *neighbour_state, const GameState *current_state, int row, int col);
void solve_sudoku_sa(Game *game, double T_start, double T_end, double alpha);

#endif //SOLVER_H
