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
        conflicts += count[num] - 1;
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
        conflicts += count[num] - 1;
      }
    }
  }

  return conflicts;
}

int generateNeighbour(GameState *neighbour_state, const GameState *current_state, int row, int col) {
   // copy 'solution' and 'frozen' arrays from current_state to neighbour_state
   for (int i = 0; i < 9; i++) {
     for (int j = 0; j < 9; j++) {
       neighbour_state -> solution[i][j] = current_state-> solution[i][j];
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
     if ((r1 != r2 || c1 != c2) && !neighbour_state->frozen[r1][c1] && !neighbour_state->frozen[r2][c2]) {
       // swap two values
       swap(&neighbour_state->solution[r1][c1], &neighbour_state->solution[r2][c2]);
       return 1;  // neighbour is taken into account (valid neighbour was generated)
     }
  }
  return 0; // FAILED to generate a valid neighbour (not taken into account)
}

// main SA function
void solve_sudoku_sa(Game *game, double T_start, double T_end, double alpha) {
    srand(time(NULL));

    // allocating memory
    int **current = allocateBoard();
    int **best = allocateBoard();
    int **neighbor = allocateBoard();

    // initializing
    copyBoard(game->board, current);
    fill_board_with_nums(current);
    copyBoard(current, best);

    int current_energy = calculateEnergy(current);
    int best_energy = current_energy;

    double T = T_start;
    int iteration = 0;
    const int max_iterations = 1000000;

    int last_improvement = 0;
    int frozen[9][9] = {0};

    // mark frozen cells (original numbers)
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            frozen[i][j] = (game->board[i][j] != 0);
        }
    }

    while (T > T_end && iteration < max_iterations && best_energy > 0) {
        iteration++;

        // generate neighbor by swapping two non-frozen cells in a random 3x3 block
        copyBoard(current, neighbor);

        int block_row = rand() % 3;
        int block_col = rand() % 3;
        int candidates[9][2];
        int count = 0;

        // find swappable cells in this block
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int r = block_row*3 + i;
                int c = block_col*3 + j;
                if (!frozen[r][c]) {
                    candidates[count][0] = r;
                    candidates[count][1] = c;
                    count++;
                }
            }
        }

        // perform swap if possible
        if (count >= 2) {
            int a = rand() % count;
            int b;
            do { b = rand() % count; } while (b == a);

            swap(&neighbor[candidates[a][0]][candidates[a][1]],
                 &neighbor[candidates[b][0]][candidates[b][1]]);
        }

        int neighbor_energy = calculateEnergy(neighbor);
        int delta = neighbor_energy - current_energy;

        // acceptance decision
        if (delta < 0 || (T > 0 && (double)rand()/RAND_MAX < exp(-delta/T))) {
            copyBoard(neighbor, current);
            current_energy = neighbor_energy;

            if (current_energy < best_energy) {
                copyBoard(current, best);
                best_energy = current_energy;
                last_improvement = iteration;
               // printf("Iteration: %d, Energy: %d, T: %.2f\n", iteration, best_energy, T);
            }
        }

        // cooling
        T *= alpha;
    }

    // copy best solution back (preserving original numbers)
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!frozen[i][j]) {
                game->board[i][j] = best[i][j];
            }
        }
    }

    printf("\nFINAL COST: %d\n", best_energy);
    //printf("Iterations: %d\n", iteration);

    // freeing memory
    free(current);
    free(best);
    free(neighbor);
}