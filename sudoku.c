#include "sudoku.h"

// DIGIT VALIDATION
// check if provided number is safe to put in row i
bool unUsedInRow(int **board, int i, int number) {
  for (int j = 0; j < 9; j++) {
    if (board[i][j] == number) {
      return false;
    }
  }
  return true;
}

// check if provided number is safe to put in column j
bool unUsedInColumn(int **board, int j, int number) {
  for (int i = 0; i < 9; i++) {
    if (board[i][j] == number) {
      return false;
    }
  }
  return true;
}

// check if 3x3 box contains number -> if it does, return false (meaning the number CANNOT be put in this box)
bool unUsedInBox(int **board, int i, int j, int number) {
  int startRow = i - i % 3;
  int startColumn = j - j % 3;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (board[startRow + i][startColumn + j] == number) {
        return false; // number already exists in the box
      }
    }
  }
  return true; // number can be put in the box
}

// final check if the number isn't used in row, column or box
// (meaning if it's safe to put the number in choosen cell)
bool finalCheck(int **board, int i, int j, int number) {
  return (unUsedInRow(board, i, number) && (unUsedInColumn(board, j, number))
          && (unUsedInBox(board, i - i % 3, j - j % 3, number)));
}

// GENERATING SUDOKU + FILLING THE BOARD
// filling 3x3 matrix
void fillBox(int **board, int row, int col) {
  int number;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      do {
        number = rand() % 9 + 1; // digits between 1-9
      }
      while (!unUsedInBox(board, row, col, number));

      board[row+i][col+j] = number;
    }
  }
}

// fill diagonal (upper left, middle, lower right) 3x3 boxes
void fillDiagonal(int **board) {
  for (int i = 0; i < 9; i=i+3) {
    fillBox(board, i, i); // row = col in diagonal
  }
}

bool fillRemaining(int **board, int i, int j) {
  // return true if you reach the end of the grid
  if (i >= 9) {
    return true;
  }

  // move to the next row when you finish filling the row
  if (j >= 9) {
    return fillRemaining(board, i+1, 0);
  }

  // skip cell that is already filled
  if (board[i][j] != 0) {
    return fillRemaining(board, i, j+1);
  }

  // try inserting numbers 1-9
  for (int number = 1; number <= 9; number++) {
    // check if the number can be put in the cell
    if (finalCheck(board, i, j, number)) {
      board[i][j] = number;
      // recursion used for filling next cells of the board
      if (fillRemaining(board, i, j+1)) {
        // if the whole board has been filled properly - stop
        return true;
      }
      // if placing the number doesn't lead to a valid solution
      // remove it from the cell and try another number
      board[i][j] = 0;
    }
  }
  return false;
}

// remove x random digits from the board (set them to 0)
void removeXDigits(int **board, int x) {
  if (board == NULL) {
    return;
  }

  int counter = 0;

  while (counter < x) {
    const int i = rand() % 9;
    const int j = rand() % 9;

    if (board[i][j] != 0) {
      board[i][j] = 0;
      counter++;
    }
  }

}

// depending of difficulty - get the number of cells to remove from the board
int getNumberOfCellsToRemove(Difficulty difficulty) {
  switch (difficulty) {
    case EASY:
      return 41;
      break;
    case MEDIUM:
      return 48;
      break;
    case HARD:
      return 55;
      break;
    default:
      return 0;
  }
  return 0;
}

int **generateSudoku(Difficulty difficulty, Game *game) {
  int x = getNumberOfCellsToRemove(difficulty);
  int **board = allocateBoard();
  if (board == NULL) {
    return NULL;
  }

  // fill the board with zeros
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      board[i][j] = 0;
    }
  }

  // 1. fill diagonal 3x3 boxes (upper left, middle, lower right)
  fillDiagonal(board);

  // 2. fill remaining boxes on the board
  if (!fillRemaining(board, 0, 0)) {
    freeGame(game);
    return generateSudoku(difficulty, game); // retrying if fails to fill the remaining
  }

  // 3. remove x random digits

  removeXDigits(board, x);

  return board;
}
