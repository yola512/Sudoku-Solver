#include "solver.h"
#include "utils.h"

// calculate cost/energy/conflicts
int calculateEnergy(int **board) {
  int conflicts = 0;

  // number of conflicts in rows:
  for (int i = 0; i < 9; i++) {
    // matrix to store how many times digits from 1-9 appeared in a row
    int count[10] = {0};
    for (int j = 0; j < 9; j++) {
      count[board[i][j]]++;
    }
    for (int num = 1; num <= 9; num++) {
      if (count[num] > 1) {
        conflicts++;
      }
    }
  }

  // number of conflicts in columns:
  for (int j = 0; j < 9; j++) {
    // matrix to store how many times digits from 1-9 appeared in a column
    // count[1] - how many times '1' appeared in a column etc
    int count[10] = {0};
    for (int i = 0; i < 9; i++) {
      count[board[i][j]]++;
    }
    for (int num = 1; num <= 9; num++) {
      if (count[num] > 1) {
        conflicts++;
      }
    }
  }

  // number of conflicts in 3x3 boxes (how many times there is a number duplicate)
  for (int boxRow = 0; boxRow < 3; boxRow++) {
    for (int boxCol = 0; boxCol < 3; boxCol++) {
      // matrix to store how many times digits from 1-9 appeared in a row
      int count[10] = {0};
      // loop for moving inside a box
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          // [boxRow * 3 + i][boxCol * 3 + j] - actual row and column indexes on the Sudoku board
          // e.g. boxRow = 1, boxCol = 2 -> box on the right, middle row
          // val - value inside the cell
          int val = board[boxRow * 3 + i][boxCol * 3 + j];
          count[val]++;
        }
      }
      for (int num = 1; num <= 9; num++) {
        if (count[num] > 1) {
          conflicts += count[num] - 1;
        }
      }
    }
  }
  return conflicts;
}

// auxiliary function for swapping two values
void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

int generateNeighbour(GameState *neighbour_state, const GameState *current_state, int row, int col) {
   // copy 'solution' and 'frozen' arrays from current_state to neighbour_state
   for (int i = 0; i < 9; i++) {
     for (int j = 0; j < 9; j++) {
       neighbour_state -> solution[i][j] = current_state-> solution[i][j];
       neighbour_state -> frozen[i][j] += current_state-> frozen[i][j];
     }
   }
   neighbour_state -> energy = current_state -> energy;

   // choose random 3x3 box
   int boxRow = rand() % 3;
   int boxCol = rand() % 3;

   int r1, r2, c1, c2;

   // max number of attempts to find 2 different, non-frozen cells in the box
   int maxAttempts = 100;

   // choose 2 random cells which are not frozen and swap their values
   for (int attempt = 0; attempt < maxAttempts; attempt++) {
     // random positions (rows, columns) within the selected 3x3 box
     r1 = boxRow * 3 + rand() % 3;
     r2 = boxRow * 3 + rand() % 3;
     c1 = boxCol * 3 + rand() % 3;
     c2 = boxCol * 3 + rand() % 3;

     // ensure positions are different and not frozen
     if ((r1 != r2) || (c1 != c2) &&
         !neighbour_state->frozen[r1][c1] &&
         !neighbour_state->frozen[r2][c2])
     {
         // swap two values
         swap(&neighbour_state->solution[r1][c1], &neighbour_state->solution[r2][c2]);
         return 1; // neighbour is taken into account (valid neighbour was generated)
     }
  }
  return 0; // FAILED to generate a valid neighbour (not taken into account)
}

// main function SA for solving sudoku
void solve_sudoku_sa(Game *game, double T_start, double T_end, double alpha) {
  srand(time(NULL));

  // allocate, initialize GameStates
  GameState current_state, neighbour_state, best_state;

  current_state.solution = allocateBoard();
  neighbour_state.solution = allocateBoard();
  best_state.solution = allocateBoard();

  current_state.frozen = allocateBoard();
  neighbour_state.frozen = allocateBoard();
  best_state.frozen = allocateBoard();

  if (!current_state.solution || !neighbour_state.solution || !best_state.solution
      || !current_state.frozen || !neighbour_state.frozen || !best_state.frozen)
  {
      perror("Failed to allocate memory for GameStates");
      // free whatever has been allocated successfully
      free(current_state.solution);
      free(neighbour_state.solution);
      free(best_state.solution);
      free(current_state.frozen);
      free(neighbour_state.frozen);
      free(best_state.frozen);
      return;
  }

  // initialize energy
  current_state.energy = calculateEnergy(current_state.solution);
  best_state.energy = current_state.energy;

  // copy current state to best_state
  copyBoard(best_state.solution, current_state.solution);
  copyBoard(best_state.frozen, current_state.frozen);
  best_state.energy = current_state.energy;

  double T = T_start;

  while (T > T_end && current_state.energy > 0) {
    // generate neighbour by swapping two non-frozen values in a random block
    int valid = generateNeighbour(&neighbour_state, &current_state, 0, 0);
    if (!valid) continue; // skip if failed to generate valid neighbour

    // calculating neighbour's + energy difference
    neighbour_state.energy = calculateEnergy(neighbour_state.solution);
    double deltaE = neighbour_state.energy - current_state.energy;

    // acceptance decision
    if (deltaE < 0) {
      // neighbour's solution is better (lower energy) -> always accept
      copyBoard(current_state.solution, neighbour_state.solution);
      copyBoard(current_state.frozen, neighbour_state.frozen);
      current_state.energy = neighbour_state.energy;
    }
    else {
      // neighbour's solution is worse or the same -> accept with exp(-deltaE/T) probability
      double probability = exp(-deltaE / T);
      if ((double)rand() / RAND_MAX < probability) {
        copyBoard(current_state.solution, neighbour_state.solution);
        copyBoard(current_state.frozen, neighbour_state.frozen);
        current_state.energy = neighbour_state.energy;
      }
      // if not accepted -> current_state stays the same
    }

    // refresh best found solution (best_state)
    if (current_state.energy < best_state.energy) {
      copyBoard(best_state.solution, current_state.solution);
      copyBoard(best_state.frozen, current_state.frozen);
      best_state.energy = current_state.energy;
    }

    // copy the best state to the game board
    copyBoard(game-> board, best_state.solution);

    // cooling
    T *= alpha;
  }
  // print final result
  printf("\nFinished.\n");
  printf("Best State Found: Energy=%.2f\n", best_state.energy);
  printf("Solution:\n");
  printBoard(game);

  // free allocated memory
  free(current_state.solution);
  free(neighbour_state.solution);
  free(best_state.solution);
  free(current_state.frozen);
  free(neighbour_state.frozen);
  free(best_state.frozen);
}