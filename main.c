#include <stdio.h>
#include "sudoku.h"
#include "solver.h"

int main(void) {
    printf("dzialam:)");
    Game *currentGame = NULL;
    printf("dzialam:)");
    // SA initialization
    double T_start = 1000.0; // wyższa początkowa temperatura
    double alpha = 0.99; // współczynnik chłodzenia
    double T_end = 0.01; // końcowa temperatura
    printf("dzialam:)");
    Difficulty difficulty = MEDIUM;  // Możesz zmienić na EASY, MEDIUM lub HARD
    printf("dzialam");
    currentGame = createNewGame(difficulty);  // Tworzymy nową grę
    printf("niewiemczydzialam");
    currentGame->board = generateSudoku(difficulty, currentGame);  // Generujemy planszę
    printf("a tu nie wiem");
    // wyświetlamy wygenerowaną planszę Sudoku przed rozwiązaniem
    printf("Generated Sudoku Board (before solving):\n");
    printBoard(currentGame);

    // uruchamiamy algorytm Simulated Annealing do rozwiązania Sudoku
    solve_sudoku_sa(currentGame, T_start, T_end, alpha);

    // wyświetlamy rozwiązanie po przeprowadzeniu algorytmu SA
    printf("Solved Sudoku Board (after SA):\n");
    printBoard(currentGame);

    // zwolnienie pamięci
    if (currentGame) {
        freeGame(currentGame);
    }

    return 0;
}